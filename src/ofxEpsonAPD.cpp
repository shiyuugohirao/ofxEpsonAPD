#include "ofxEpsonAPD.h"

HANDLE	hAsync;
DWORD	g_dwStatus;

int CALLBACK CBGetStatus(DWORD dwStatus){
	g_dwStatus = dwStatus;

	if (g_dwStatus & ASB_PRINT_SUCCESS || g_dwStatus & ASB_NO_RESPONSE ||
		g_dwStatus & ASB_COVER_OPEN || g_dwStatus & ASB_AUTOCUTTER_ERR ||
		g_dwStatus & ASB_RECEIPT_NEAR_END) {
		SetEvent(hAsync);
	}
	return SUCCESS;
}

/* ofxEpsonAPD
------------------------------------------------------------*/
ofxEpsonAPD::ofxEpsonAPD(string IpName) {
	hAsync = CreateEvent(
		NULL								/*lpEventAttributes*/, 
		FALSE								/*bManualReset*/, 
		FALSE								/*bInitialState*/, 
		to_wstring(IpName).c_str()	/*lpName*/
	);

	startThread();
}

ofxEpsonAPD::~ofxEpsonAPD(){
		CloseHandle(hAsync);
		waitCallbackChannel.close();
		waitForThread(true);
}
void ofxEpsonAPD::setup(string printerName, string docName) {
	defaultPrinterName = printerName;
	defaultDocName = docName;
}
void ofxEpsonAPD::setDefaultPrinterName(string printerName) {
	defaultPrinterName = printerName;
}
void ofxEpsonAPD::setDefaultDocName(string docName) {
	defaultDocName = docName;
}
void ofxEpsonAPD::setUseCallback(bool useCallback) {
	this->useCallback = useCallback;
}

void ofxEpsonAPD::begin() {

	begin(defaultPrinterName, defaultDocName);
}
void ofxEpsonAPD::begin(string printerName, string docName) {
	if (begining) {
		ofLogWarning("ofxEpsonAPD", "already begin(), did you forget to end() somewhere? ");
	}
	begining = true;

	if (useCallback) {
		if (m_statAPI.Initialize() == FALSE) {
			AfxMessageBox(_T("StatusAPI�̃��[�h�Ɏ��s���܂����B"));
			return;
		}

		// BiOpenMonPrinter�̎��s
		nHandle = m_statAPI.BiOpenMonPrinter(TYPE_PRINTER, (LPSTR)printerName.c_str());
		if (nHandle <= 0)
		{
			AfxMessageBox(_T("�Ď��v�����^�̃I�[�v���Ɏ��s���܂����B"));
			return;
		}

		// BiSetStatusBackFunction�̎��s
		if (m_statAPI.BiSetStatusBackFunction(nHandle, CBGetStatus) != SUCCESS)
		{
			AfxMessageBox(_T("�R�[���o�b�N�֐��̓o�^�Ɏ��s���܂����B"));
			// BiCloseMonPrinter�̎��s
			m_statAPI.BiCloseMonPrinter(nHandle);
			return;
		}
	}

	if (!dc.CreateDC(
		NULL,								/* (LPCWSTR)pwszDriver */
		to_wstring(printerName).c_str(),	/* (LPCWSTR)pwszDevice */
		NULL,								/* (LPCWSTR)pszPort */
		NULL))	
	{
		AfxMessageBox(_T("�v�����^�𗘗p�ł��܂���B"));
		// BiCloseMonPrinter�̎��s
		if (m_statAPI.BiCloseMonPrinter(nHandle) != SUCCESS) {
			AfxMessageBox(_T("�Ď��v�����^�̃N���[�Y�Ɏ��s���܂����B"));
		}
		return;
	}

	// ����J�n
	printing = true;
	if (useCallback) waitingCallback = true;
	dc.StartDoc(to_wstring(docName).c_str());
	dc.StartPage();
}

void ofxEpsonAPD::end() {
	if (!begining) {
		ofLogWarning("ofxEpsonAPD", "not begin(), did you forget to begin() somewhere? ");
	}

	dc.EndPage();
	// ����I��
	dc.EndDoc();
	dc.DeleteDC();
	printing = false;
	begining = false;
	

	waitCallbackChannel.send(useCallback);
}


void ofxEpsonAPD::drawImage(ofPixels pix) {
	drawImage(0, 0, pix);
}
void ofxEpsonAPD::drawImage(ofTexture tex) {
	drawImage(0, 0, tex);
}
void ofxEpsonAPD::drawImage(int x, int y, ofTexture tex) {
	ofPixels pix;
	tex.readToPixels(pix);
	drawImage(x, y, pix);
}
void ofxEpsonAPD::drawImage(int x, int y, ofPixels pix) {
	if (!pix.isAllocated()) {
		ofLogWarning() << "pix is not allocated.";
		return;
	}
	LPBITMAPINFO info;
	HBITMAP      hbit;
	BITMAP       bm;
	int          nColors = 0;
	int          sizeinfo = 0;
	RGBQUAD      rgb[256];
	pix.setImageType(ofImageType::OF_IMAGE_COLOR); // remove AlphaChannel
	pix.mirror(true, false); // fix direction
	unsigned char* buffer = pix.getData();

	int bufferSize = pix.size();
	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),
			pix.getWidth(), pix.getHeight(), 1,
			24 /* biBitCount (1, 4, 8, 16, 24, 32) */,
			BI_RGB,bufferSize, 0, 0, 0, 0 };
	hbit = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&buffer, 0, 0);
	if (hbit == NULL) {
		DWORD lastError = GetLastError();
		dc.AbortDoc();
		//dc.DeleteDC();
		return;
	}
	memcpy(buffer, pix.getData(), bufferSize);

	// 'hbit'�̏����擾����'bm'�ɕۑ�
	GetObject(hbit, sizeof(BITMAP), (LPVOID)&bm);

	nColors = (1 << bm.bmBitsPixel);
	if (nColors > 256) nColors = 0;

	sizeinfo = sizeof(BITMAPINFO) + (nColors * sizeof(RGBQUAD));  // �r�b�g�}�b�v���̃T�C�Y
	info = (LPBITMAPINFO)malloc(sizeinfo);                        // �r�b�g�}�b�v�����������ɕۑ�

	info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biWidth = bm.bmWidth;
	info->bmiHeader.biHeight = bm.bmHeight;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biBitCount = bm.bmBitsPixel * bm.bmPlanes;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biSizeImage = bm.bmWidthBytes * bm.bmHeight;
	info->bmiHeader.biXPelsPerMeter = 0;
	info->bmiHeader.biYPelsPerMeter = 0;
	info->bmiHeader.biClrUsed = 0;
	info->bmiHeader.biClrImportant = 0;

	if (nColors <= 256)
	{
		HBITMAP hOldBitmap;
		HDC     hMemDC = CreateCompatibleDC(NULL);      // �f�o�C�X�R���e�L�X�g�̍쐬

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hbit);  // �f�o�C�X�R���e�L�X�g��'hbit'��I��'
		GetDIBColorTable(hMemDC, 0, nColors, rgb);          // �J���[�e�[�u�����̎擾

		// �F����"info"�ɐݒ�
		for (int iCnt = 0; iCnt < nColors; ++iCnt)
		{
			info->bmiColors[iCnt].rgbRed = rgb[iCnt].rgbRed;
			info->bmiColors[iCnt].rgbGreen = rgb[iCnt].rgbGreen;
			info->bmiColors[iCnt].rgbBlue = rgb[iCnt].rgbBlue;
		}
		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	
	// �r�b�g�}�b�v�̈��
	StretchDIBits(dc.GetSafeHdc(),
		x, y,							/* position */
		bm.bmWidth, bm.bmHeight,		/* size */
		0, 0, bm.bmWidth, bm.bmHeight,	/* trim */
		bm.bmBits,
		info,
		DIB_RGB_COLORS, SRCCOPY);

	DeleteObject(hbit);
	free(info);
}

void ofxEpsonAPD::drawText(int x, int y, string text, int pointSize, string fontName) {
	CFont font, * old;
	font.CreatePointFont(pointSize, to_wstring(fontName).c_str(), &dc);
	old = dc.SelectObject(&font);
	dc.TextOut(x, y, to_wstring(text).c_str());
	dc.SelectObject(old);
	font.DeleteObject();
}

void ofxEpsonAPD::pushFontStyle(int pointSize, string fontName) {
	if (pushingFontStyle) {
		ofLogWarning("ofxEpsonAPD", "already pushFontStyle(), did you forget to pop somewhere? ");
	}
	pushingFontStyle = true;
	defaultFont.CreatePointFont(pointSize, to_wstring(fontName).c_str(), &dc);
	defaultOld = dc.SelectObject(&defaultFont);
}
void ofxEpsonAPD::popFontStyle() {
	if (!pushingFontStyle) {
		ofLogWarning("ofxEpsonAPD", "not pushFontStyle(), did you forget to push somewhere? ");
	}
	pushingFontStyle = false;
	dc.SelectObject(defaultOld);
	defaultFont.DeleteObject();
}
void ofxEpsonAPD::drawText(int x, int y, string text) {
	dc.TextOut(x, y, to_wstring(text).c_str());
}


void ofxEpsonAPD::threadedFunction() {
	bool usedCallback;
	while (waitCallbackChannel.receive(usedCallback)) {
		if (!usedCallback) break;
		// �v�����^����X�e�[�^�X���擾����܂őҋ@
		WaitForSingleObject(hAsync, INFINITE);
		dwPrintStatus = g_dwStatus;

		// BiCancelStatusBack�̎��s
		int r = m_statAPI.BiCancelStatusBack(nHandle);

		// ��������i����I���j
		if (dwPrintStatus & ASB_PRINT_SUCCESS) {
			//AfxMessageBox(_T("�󎚂��I�����܂����B"), MB_ICONINFORMATION);
			ofLogNotice("ofxEpsonAPD", "�󎚂��I�����܂����B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, PRINT_SUCCESS);
			ofNotifyEvent(callbackEvent, e);
		}
		// �ʐM�G���[
		else if (dwPrintStatus & ASB_NO_RESPONSE) {
			//AfxMessageBox(_T("�v�����^�̉���������܂���B"));
			ofLogError("ofxEpsonAPD", "�v�����^�̉���������܂���B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, PRINT_SUCCESS);
			ofNotifyEvent(callbackEvent, e);
			m_statAPI.BiCancelError(nHandle);
		}
		// �J�o�[�I�[�v��
		else if (dwPrintStatus & ASB_COVER_OPEN) {
			//AfxMessageBox(_T("�J�o�[���J���Ă��܂��B"));
			ofLogError("ofxEpsonAPD", "�J�o�[���J���Ă��܂��B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, COVER_OPEN);
			ofNotifyEvent(callbackEvent, e);
			m_statAPI.BiCancelError(nHandle);
		}
		// �J�b�g�G���[
		else if (dwPrintStatus & ASB_AUTOCUTTER_ERR) {
			//AfxMessageBox(_T("�J�b�g�G���[���������܂����B"));
			ofLogError("ofxEpsonAPD", "�J�b�g�G���[���������܂����B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, AUTOCUTTER_ERR);
			ofNotifyEvent(callbackEvent, e);
			m_statAPI.BiCancelError(nHandle);
		}
		// ���Ȃ�
		else if (dwPrintStatus & ASB_RECEIPT_NEAR_END) {
			//AfxMessageBox(_T("���[�������Ȃ��Ȃ�܂����B"));
			ofLogWarning("ofxEpsonAPD", "���[�������Ȃ��Ȃ�܂����B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, RECEIPT_NEAR_END);
			ofNotifyEvent(callbackEvent, e);
			m_statAPI.BiCancelError(nHandle);
		}

		// BiCloseMonPrinter�̎��s
		if (m_statAPI.BiCloseMonPrinter(nHandle) != SUCCESS) {
			//AfxMessageBox(_T("�Ď��v�����^�̃N���[�Y�Ɏ��s���܂����B"));
			ofLogError("ofxEpsonAPD", "�Ď��v�����^�̃N���[�Y�Ɏ��s���܂����B");
			PrinterCallbackEvent e(defaultPrinterName, defaultDocName, OTHER_ERROR);
			ofNotifyEvent(callbackEvent, e);
		}

		waitingCallback = false;

	}
}


/* ofxPrinterUtils
------------------------------------------------------------*/

// --- getLocalPrinterNames()
// refer to http://yamatyuu.net/computer/program/sdk/base/enumprn/index.html
vector<string> ofxPrinterUtils::getLocalPrinterNames() {
	vector<string> printerNames;
	DWORD dwNeeded, dwRet, i;
	PRINTER_INFO_1* prnInfo;
	LPBYTE pPrnEnum;
	//	�v�����^�̖��O���ɕK�v�ȃo�C�g�����擾
	EnumPrinters(PRINTER_ENUM_LOCAL,//	�����[�J���v�����^��Ώ�
		NULL, 1, NULL, 0, &dwNeeded, &dwRet);
	pPrnEnum = (LPBYTE)LocalAlloc(LPTR, dwNeeded);// ���������m��
	if (pPrnEnum == NULL) {
		return printerNames;
	}
	//	�v�����^�̏����擾
	EnumPrinters(PRINTER_ENUM_LOCAL, NULL, 1, pPrnEnum, dwNeeded, &dwNeeded, &dwRet);
	prnInfo = (PRINTER_INFO_1*)pPrnEnum;
	cout << "=== getLocalPrinterNames ===" << endl;
	for (i = 0; i < dwRet; i++) {
		string printer = to_string(prnInfo[i].pName);
		printerNames.push_back(printer);
		getPrinterInfo(printer);
	}
	LocalFree(pPrnEnum);	//	�����������

	return printerNames;
}

bool ofxPrinterUtils::getPrinterInfo(const string printerName, PRINTER_INFO_1W* pInfo) {
	HANDLE hPrinter;
	LPBYTE pPrinter;
	DWORD dwNeeded;

	if (!OpenPrinter((LPWSTR)to_wstring(printerName).c_str(), &hPrinter, NULL)) {
		return false;
	}

	GetPrinter(hPrinter, 1, NULL, 0, &dwNeeded);

	pPrinter = (LPBYTE)LocalAlloc(LPTR, dwNeeded);
	if (pPrinter == NULL) {
		ClosePrinter(hPrinter);
		return false;
	}

	GetPrinter(hPrinter, 1, pPrinter, dwNeeded, &dwNeeded);

	wprintf(L"Name			: %s\n", ((PRINTER_INFO_1W*)pPrinter)->pName);
	wprintf(L"Description	: %s\n", ((PRINTER_INFO_1W*)pPrinter)->pDescription);
	wprintf(L"Comment		: %s\n\n", ((PRINTER_INFO_1W*)pPrinter)->pComment);

	if (pInfo != nullptr) {
		pInfo = (PRINTER_INFO_1W*)pPrinter;
	}

	ClosePrinter(hPrinter);
	LocalFree(pPrinter);
	return true;
}
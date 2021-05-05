#pragma once

/// [ NOTICE!!! ]
///	Need to comment out `#include "ofMain.h"` in main.cpp, and
/// Need to add `#include"ofxEpsonAPD.h"` before include "ofMain.h"(because of windows.h)
/// https://forum.openframeworks.cc/t/how-to-include-properly-afxwin-h-to-a-vs-project/4329
///

#include <afxwin.h>			// MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <winspool.h>
#include "StatusAPI.h"
#include "EpsStmApi.h"

#include "ofMain.h"
#include "wstringUtils.h"

/// <summary>
/// ofxEpsonAPD with Win32API
/// openFrameworks addons to control EponPrinter
/// using Epson AdvancedPrinterDriver
/// </summary>
class ofxEpsonAPD : ofThread {

private:
	CDC dc;
	int nHandle;
	CStatusAPI	m_statAPI;
	DWORD	dwPrintStatus;

	string defaultPrinterName;
	string defaultDocName;
	CFont defaultFont, * defaultOld;

	bool useCallback;
	bool printing;
	bool waitingCallback;
	bool begining, pushingFontStyle;

	ofThreadChannel<bool> waitCallbackChannel;

	void threadedFunction();


public:
	ofxEpsonAPD(string IpName = "StatusEvent");
	~ofxEpsonAPD();


	void setup(string printerName, string docName);
	void setDefaultPrinterName(string printerName);
	void setDefaultDocName(string docName);
	void setUseCallback(bool useCallback);

	void begin();
	void begin(string printerName, string docName);
	void end();

	void drawImage(ofPixels pix);
	void drawImage(ofTexture tex);
	void drawImage(int x, int y, ofPixels pix);
	void drawImage(int x, int y, ofTexture tex);

	/// <summary>
	/// drawText
	/// </summary>
	/// <param name="font"> FontA11,FontA22, FontA12,FontA21, Japanese11 Japanese22 </param>
	void drawText(int x, int y, string text, int pointSize, string font = "FontA11");

	void pushFontStyle(int pointSize, string font = "FontA11");
	void popFontStyle();
	void drawText(int x, int y, string text);

	bool isPrinting() const { return printing; }
	bool isWaitingCallback() const { return waitingCallback; }

	enum PRINT_STATUS {
		PRINT_SUCCESS,
		NO_RESPONSE,
		COVER_OPEN,
		AUTOCUTTER_ERR,
		RECEIPT_NEAR_END,
		OTHER_ERROR,
	};
	struct PrinterCallbackEvent {
		string printerName;
		string docName;
		PRINT_STATUS printerStatus;
		PrinterCallbackEvent(string p, string d, PRINT_STATUS s)
			:printerName(p), docName(d), printerStatus(s) {};
	};
	ofEvent<PrinterCallbackEvent> callbackEvent;
};


/// <summary>
/// PrinterUtils
/// </summary>
static class ofxPrinterUtils {
public:
	static vector<string> getLocalPrinterNames();
	static bool getPrinterInfo(const string printerName, PRINTER_INFO_1W* pInfo = nullptr);
};

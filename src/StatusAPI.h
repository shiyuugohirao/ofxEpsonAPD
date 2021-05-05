/*
StatusAPI.h

Class definition for Status API. Includes the functions required for CompletionDialog
*/

#ifndef	_STATUS_API
#define _STATUS_API

#include "EpsStmApi.h"

#define STATUS_API_FILE	"EpsStmApi.dll"

#define MC_AUTOCUT_OPERATIONS	0x32

class CStatusAPI {
protected:
	// Handle to a DLL
	HMODULE m_hDll;

private:
	// Function pointers
	DLL_BiOpenMonPrinter	m_funcBiOpenMonPrinter;
	DLL_BiCloseMonPrinter	m_funcBiCloseMonPrinter;
	DLL_BiGetStatus			m_funcBiGetStatus;
	DLL_BiCancelStatusBack	m_funcBiCancelStatusBack;
	DLL_BiResetPrinter		m_funcBiResetPrinter;
	DLL_BiCancelError		m_funcBiCancelError;
	DLL_BiSetStatusBackFunction	m_funcBiSetStatusBackFunction;
	DLL_BiOpenDrawer		m_funcBiOpenDrawer;
	DLL_BiLockPrinter		m_funcBiLockPrinter;
	DLL_BiUnlockPrinter		m_funcBiUnlockPrinter;
	DLL_BiGetType			m_funcBiGetType;
	DLL_BiGetPrnCapability	m_funcBiGetPrnCapability;
	DLL_BiGetCounter		m_funcBiGetCounter;


public:
	// Constructor and Destructor
	CStatusAPI(void);
	virtual ~CStatusAPI();

	// Public API Functions. Must call Initialize() before anything else
	BOOL	Initialize();
	int BiOpenMonPrinter(int nType, LPSTR pName);
	int BiCloseMonPrinter(int nHandle);
	int BiGetStatus(int nHandle, LPDWORD lpStatus);
	int	BiCancelStatusBack(int nHandle);
	int BiResetPrinter(int nHandle);
	int BiCancelError(int nHandle);
	int BiSetStatusBackFunction (int nHandle, int (CALLBACK EXPORT *pStatusCB)(DWORD dwStatus));
	int BiOpenDrawer( int nHandle, BYTE drawer, BYTE pulse);
	int	BiUnlockPrinter(int nHandle);
	int	BiLockPrinter(int nHandle, DWORD timeout);
	int BiGetPrnCapability( int nHandle, BYTE prnID, LPBYTE pBuffSize, LPBYTE pBuff);
	int BiGetType(int nHandle, LPBYTE type_id, LPBYTE font,
		LPBYTE exrom, LPBYTE euspecial);
	int BiGetCounter(int nHandle, BYTE readno, LPDWORD readcounter);
};

#endif
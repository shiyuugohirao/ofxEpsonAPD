/*
 * StatusAPI.cpp
 * Function definitions for components of the Status API
 */
//#include "stdafx.h"
#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdisp.h>        // MFC Automation classes
#include "EpsStmApi.h"
#include "StatusAPI.h"

CStatusAPI::CStatusAPI():m_hDll(NULL) {
	// Initialize all to NULL
	m_hDll = NULL;
	m_funcBiOpenMonPrinter		= NULL;
	m_funcBiOpenMonPrinter		= NULL;
	m_funcBiCloseMonPrinter		= NULL;
	m_funcBiGetStatus			= NULL;
	m_funcBiCancelStatusBack	= NULL;
	m_funcBiResetPrinter		= NULL;
	m_funcBiCancelError			= NULL;
	m_funcBiSetStatusBackFunction = NULL;
	m_funcBiOpenDrawer			= NULL;
	m_funcBiLockPrinter			= NULL;
	m_funcBiUnlockPrinter		= NULL;
	m_funcBiGetType				= NULL;
	m_funcBiGetPrnCapability	= NULL;
	m_funcBiGetCounter			= NULL;
}

CStatusAPI::~CStatusAPI(void) {
	if ( m_hDll != NULL ) {
		::FreeLibrary(m_hDll);
	}
}

BOOL	CStatusAPI::Initialize() {
	/*
	 * Load the Status API DLL
	 */
	m_hDll = ::LoadLibrary(_T("EpsStmApi.dll"));
	if ( m_hDll != NULL ) {
		// Load the necessary functions for this application
		m_funcBiOpenMonPrinter 
			= (DLL_BiOpenMonPrinter)GetProcAddress(m_hDll, "BiOpenMonPrinter");
		m_funcBiCloseMonPrinter 
			= (DLL_BiCloseMonPrinter)GetProcAddress(m_hDll, "BiCloseMonPrinter");
		m_funcBiGetStatus
			= (DLL_BiGetStatus)GetProcAddress(m_hDll, "BiGetStatus");
		m_funcBiCancelStatusBack
			= (DLL_BiCancelStatusBack)GetProcAddress(m_hDll, "BiCancelStatusBack");
		m_funcBiResetPrinter
			= (DLL_BiResetPrinter)GetProcAddress(m_hDll, "BiResetPrinter");
		m_funcBiCancelError
			= (DLL_BiCancelError)GetProcAddress(m_hDll, "BiCancelError");
		m_funcBiSetStatusBackFunction
			= (DLL_BiSetStatusBackFunction)GetProcAddress(m_hDll, "BiSetStatusBackFunction");
		m_funcBiOpenDrawer
			= (DLL_BiOpenDrawer)GetProcAddress(m_hDll, "BiOpenDrawer");
		m_funcBiLockPrinter
			= (DLL_BiLockPrinter)GetProcAddress(m_hDll, "BiLockPrinter");
		m_funcBiUnlockPrinter
			= (DLL_BiUnlockPrinter)GetProcAddress(m_hDll, "BiUnlockPrinter");
		m_funcBiGetType
			= (DLL_BiGetType)GetProcAddress(m_hDll, "BiGetType");
		m_funcBiGetPrnCapability	
			= (DLL_BiGetPrnCapability)GetProcAddress(m_hDll, "BiGetPrnCapability");
		m_funcBiGetCounter
			= (DLL_BiGetCounter)GetProcAddress(m_hDll, "BiGetCounter");

		if (m_funcBiOpenMonPrinter != NULL &&
			m_funcBiCloseMonPrinter != NULL &&
			m_funcBiGetStatus != NULL &&
			m_funcBiCancelStatusBack != NULL &&
			m_funcBiResetPrinter != NULL &&
			m_funcBiCancelError != NULL &&
			m_funcBiSetStatusBackFunction != NULL &&
			m_funcBiOpenDrawer != NULL &&
			m_funcBiLockPrinter != NULL &&
			m_funcBiUnlockPrinter != NULL &&
			m_funcBiGetType != NULL &&
			m_funcBiGetPrnCapability != NULL &&
			m_funcBiGetCounter !=NULL ) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
}

int		CStatusAPI::BiOpenMonPrinter(int nType, LPSTR pName) {
	if (  m_funcBiOpenMonPrinter!=NULL ) {
		return m_funcBiOpenMonPrinter(nType, pName);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiCloseMonPrinter(int nHandle)
{
	if (  m_funcBiCloseMonPrinter!=NULL  ) {
		return m_funcBiCloseMonPrinter(nHandle);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiGetStatus(int nHandle, LPDWORD lpStatus)
{
	if (  m_funcBiGetStatus!=NULL  ) {
		return m_funcBiGetStatus(nHandle, lpStatus);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiCancelStatusBack(int nHandle)
{
	if (  m_funcBiCancelStatusBack != NULL  ) {
		return m_funcBiCancelStatusBack(nHandle);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiResetPrinter(int nHandle)
{
	if (  m_funcBiResetPrinter != NULL  ) {
		return m_funcBiResetPrinter(nHandle);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiCancelError(int nHandle) 
{
	if (  m_funcBiCancelError != NULL  ) {
		return m_funcBiCancelError(nHandle);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiSetStatusBackFunction (int nHandle, int (CALLBACK EXPORT *pStatusCB)(DWORD dwStatus))
{
	if (  m_funcBiSetStatusBackFunction != NULL  ) {
		return m_funcBiSetStatusBackFunction (nHandle, pStatusCB);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiOpenDrawer( int nHandle, BYTE drawer, BYTE pulse)
{
	if (  m_funcBiOpenDrawer != NULL  ) {
		return m_funcBiOpenDrawer(nHandle, drawer, pulse);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiLockPrinter(int nHandle, DWORD timeout)
{
	if (  m_funcBiLockPrinter != NULL )
	{
		return m_funcBiLockPrinter(nHandle, timeout);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiUnlockPrinter(int nHandle)
{
	if ( m_funcBiUnlockPrinter != NULL  )
	{
		return m_funcBiUnlockPrinter(nHandle);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiGetPrnCapability( int nHandle, BYTE prnID, LPBYTE pBuffSize, LPBYTE pBuff)
{
	if ( m_funcBiGetPrnCapability != NULL  )
	{
		return m_funcBiGetPrnCapability(nHandle, prnID, pBuffSize, pBuff);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiGetType(int nHandle, LPBYTE type_id, LPBYTE font,
	LPBYTE exrom, LPBYTE euspecial)
{
	if ( m_funcBiGetType != NULL  )
	{
		return m_funcBiGetType(nHandle, type_id, font, exrom, euspecial);
	}
	return ERR_NO_PRINTER;
}

int		CStatusAPI::BiGetCounter(int nHandle, BYTE readno, LPDWORD readcounter)
{
	if ( m_funcBiGetCounter != NULL  )
	{
		return m_funcBiGetCounter(nHandle, readno, readcounter);
	}
	return ERR_NO_PRINTER;
}


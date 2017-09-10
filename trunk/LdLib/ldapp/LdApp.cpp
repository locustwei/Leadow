#include "stdafx.h"
#include "LdApp.h"

#define MM_CALLBACKMSG WM_USER + 0xFAB

void DebugOutput(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	LPTSTR szSprintf = NULL;
	va_list argList;
	int nLen;
	va_start(argList, pstrFormat);
	nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
	int iRet = _vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
	va_end(argList);
	::OutputDebugString(szSprintf);
	free(szSprintf);
#endif
}

namespace LeadowLib {


	CLdApp* CLdApp::ThisApp = nullptr;


	CLdApp::CLdApp() 
		:m_InstallPath()
		,m_AppDataPath()
	{
	}

	CLdString& CLdApp::GetInstallPath()
	{
		return m_InstallPath;
	}

	CLdString& CLdApp::GetAppDataPath()
	{
		return m_AppDataPath;
	}

	boolean CLdApp::Initialize(HINSTANCE hInstance)
	{
		CoInitialize(nullptr);
		ThisApp = new CLdApp();
		ThisApp->m_Instance = hInstance;
		ThisApp->m_ThreadID = GetCurrentThreadId();

		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(hInstance, tszModule, MAX_PATH);
		ThisApp->m_InstallPath = tszModule;
		int pos = ThisApp->m_InstallPath.ReverseFind(_T('\\'));
		if (pos >= 0) ThisApp->m_InstallPath = ThisApp->m_InstallPath.Left(pos + 1);

		ExpandEnvironmentStrings(_T("%APPDATA%"), tszModule, MAX_PATH);
		ThisApp->m_AppDataPath = tszModule;
		ThisApp->m_AppDataPath += _T("\\DYDOT\\");

		return true;
	}

	void CLdApp::MainThreadMessage(MSG & msg)
	{
		if (msg.message == MM_CALLBACKMSG)
		{
			IGernalCallback<LPVOID>* callback = reinterpret_cast<IGernalCallback<LPVOID>*>(msg.wParam);
			callback->GernalCallback_Callback(nullptr, msg.lParam);
		}
	}

	BOOL CLdApp::Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param)
	{
		return PostThreadMessage(ThisApp->m_ThreadID, MM_CALLBACKMSG, (WPARAM)callback, Param);
	}
}
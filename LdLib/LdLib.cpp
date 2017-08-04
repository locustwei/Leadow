#include "stdafx.h"
#include "LdLib.h"

#define MM_CALLBACKMSG WM_USER + 0xFAB

namespace LeadowLib {

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
	static CLdApp* ThisApp = nullptr;

	boolean CLdApp::Initialize(HINSTANCE hInstance)
	{
		CoInitialize(nullptr);
		ThisApp = new CLdApp();
		ThisApp->m_Instance = hInstance;
		ThisApp->m_ThreadID = GetCurrentThreadId();
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
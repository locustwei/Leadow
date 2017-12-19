#include "stdafx.h"
#include "LdApp.h"

#define MM_CALLBACKMSG WM_USER + 0xFAB  //发消息到主线程，wparam是IGernalCallback在主线程执行回掉函数


#ifdef _DEBUG
#ifdef WIN64
#define INVOKER_EXE _T("LdInvoker_d64.exe")
#else
#define INVOKER_EXE _T("LdInvoker_d32.exe")
#endif
#else
#ifdef WIN64
#else
#endif
#endif

void DebugOutput(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	LPTSTR szSprintf;
	va_list argList;
	int nLen;
	va_start(argList, pstrFormat);
	nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (TCHAR*)malloc((nLen + 7) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 7) * sizeof(TCHAR));
	_vsntprintf(szSprintf+6, nLen + 1, pstrFormat, argList);
	szSprintf[0] = '_';
	szSprintf[1] = '_';
	szSprintf[2] = 'L';
	szSprintf[3] = 'D';
	szSprintf[4] = ' ';
	szSprintf[5] = ' ';
	va_end(argList);
	::OutputDebugString(szSprintf);
	free(szSprintf);
#endif
}


CLdApp* ThisApp = nullptr;


CLdApp::CLdApp() 
	: m_Instance(nullptr)
	, m_ThreadID(0)
	, m_AppPath()
	, m_AppDataPath()
	, m_Job()
	//, m_Config()
{
}

CLdString& CLdApp::GetAppPath()
{
	return m_AppPath;
}

CLdString& CLdApp::GetAppDataPath()
{
	return m_AppDataPath;
}

HINSTANCE CLdApp::GetInstance()
{
	return m_Instance;
}

DWORD CLdApp::GetMainThreadId()
{
	return m_ThreadID;
}

BOOL CLdApp::Initialize(HINSTANCE hInstance)
{
	CoInitialize(nullptr);

	ThisApp = new CLdApp();
	ThisApp->m_Instance = hInstance;
	ThisApp->m_ThreadID = GetCurrentThreadId();

	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(hInstance, tszModule, MAX_PATH);
	ThisApp->m_AppPath = tszModule;
	int pos = ThisApp->m_AppPath.ReverseFind(_T('\\'));
	if (pos >= 0) ThisApp->m_AppPath = ThisApp->m_AppPath.Left(pos + 1);

	ExpandEnvironmentStrings(_T("%APPDATA%"), tszModule, MAX_PATH);
	ThisApp->m_AppDataPath.Format(_T("%s\\%s\\%s\\"), tszModule, COMPANY_NAME, APPLICATION_NAME);

//	CLdString FileName = ThisApp->GetAppDataPath();
//	FileName += "dydot.cng";
//
//	ThisApp->m_Config.LoadFromFile(FileName);
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

DWORD CLdApp::RunInvoker(TCHAR* Param, DWORD Flag, PVOID pContext)
{
	CLdString invoker = GetAppPath() + INVOKER_EXE;

	if (Flag & RUN_FLAG_ASADMINI)
	{
		SHELLEXECUTEINFO info = { 0 };
		info.cbSize = sizeof(SHELLEXECUTEINFO);
		info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_DEFAULT;
		info.lpVerb = TEXT("runas");
		info.lpFile = invoker;
		info.lpParameters = Param;

		if (!ShellExecuteEx(&info))
			return 0;
		PROCESS_BASIC_INFORMATION information;
		NTSTATUS ret = NtQueryInformationProcess(info.hProcess, &information, sizeof(PROCESS_BASIC_INFORMATION));
		if (!NT_SUCCESS(ret))
			return 0;
		CloseHandle(info.hProcess);
		m_Job.Put(information.UniqueProcessId, pContext);
		return information.UniqueProcessId;
	}
	else
	{
		PROCESS_INFORMATION info = { 0 };
		STARTUPINFO si = { 0 };
		si.cb = sizeof(STARTUPINFO);
		if (CreateProcess(invoker, Param, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &info))
		{
			m_Job.Put(info.dwProcessId, pContext);
			return info.dwProcessId;
		}
		else
			return 0;
	}
}

PVOID CLdApp::GetJobContext(DWORD pid)
{
	PVOID* result = nullptr;
	m_Job.GetAt(0, &result); //m_Job[pid];
	if (result)
		return *result;
	else
		return nullptr;
}

//CConfig* CLdApp::GetConfig()
//{
//	return &m_Config;
//}

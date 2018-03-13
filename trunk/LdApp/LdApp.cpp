#include "stdafx.h"
#include "LdApp.h"

#define MM_CALLBACKMSG WM_USER + 0xFAB  //����Ϣ�����̣߳�wparam��IGernalCallback�����߳�ִ�лص�����


#ifdef _DEBUG
#ifdef WIN64
#define INVOKER_EXE _T("LdInvoker_d64.exe")
#else
#define INVOKER_EXE _T("LdInvoker_d32.exe")
#endif
#else
#define INVOKER_EXE _T("LdInvoker.exe")
#endif

CLdApp* ThisApp = nullptr;

void DebugOutput(LPCTSTR pstrFormat, ...)
{
//#ifdef _DEBUG
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
	if(ThisApp)
	{
		CLdString s;
		s.Format(_T("%s %s"), ThisApp->GetAppName(), szSprintf);
		::OutputDebugString(s);
	}
	else
		::OutputDebugString(szSprintf);
	free(szSprintf);
//#endif
}

CLdApp::CLdApp() 
	: m_Instance(nullptr)
	, m_ThreadID(0)
	, m_AppPath()
	, m_AppDataPath()
	//, m_Job()
	//, m_Config()
{
}

CLdApp::~CLdApp()
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

CLdString CLdApp::GetPluginPath()
{
	return m_AppPath + _T("modules");
}

HINSTANCE CLdApp::GetInstance()
{
	return m_Instance;
}

DWORD CLdApp::GetMainThreadId()
{
	return m_ThreadID;
}

CLdStringW& CLdApp::GetAppName()
{
	if(m_AppName.IsEmpty())
	{
		//��ȡ��ִ���ļ��İ汾��Ϣ
		TCHAR tszModule[MAX_PATH + 1] = { 0 };
		::GetModuleFileName(nullptr, tszModule, MAX_PATH);
		CFileUtils::ExtractFileName(tszModule, m_AppName);		
	}
	return m_AppName;
}

BOOL CLdApp::Initialize(HINSTANCE hInstance)
{
	CoInitialize(nullptr);

	//setlocale(LC_ALL, "chs");

	ThisApp = new CLdApp();
	ThisApp->m_Instance = hInstance;
	ThisApp->m_ThreadID = GetCurrentThreadId();

	TCHAR tszModule[MAX_PATH + 1] = { 0 };
	::GetModuleFileName(hInstance, tszModule, MAX_PATH);
	ThisApp->m_AppPath = tszModule;
	int pos = ThisApp->m_AppPath.ReverseFind(_T('\\'));
	if (pos >= 0) 
		ThisApp->m_AppPath = ThisApp->m_AppPath.Left(pos + 1);

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
	return PostThreadMessage(m_ThreadID, MM_CALLBACKMSG, (WPARAM)callback, Param);
}

DWORD CLdApp::RunPluginHost(TCHAR* Param, bool admin, HANDLE* hProcess)
{
	CLdString invoker = GetAppPath() + INVOKER_EXE;
	PROCESS_INFORMATION info = { 0 };
	DWORD result = RunProcess(invoker, Param, admin, &info);
	if (result == 0 && hProcess)
		*hProcess = info.hProcess;
//		PROCESS_BASIC_INFORMATION information;
//		NTSTATUS ret = NtQueryInformationProcess(info.hProcess, &information, sizeof(PROCESS_BASIC_INFORMATION));
	return result;
}

//PVOID CLdApp::GetJobContext(DWORD pid)
//{
//	PVOID* result = nullptr;
//	m_Job.GetAt(0, &result); //m_Job[pid];
//	if (result)
//		return *result;
//	else
//		return nullptr;
//}

//CConfig* CLdApp::GetConfig()
//{
//	return &m_Config;
//}
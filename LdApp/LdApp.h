#pragma once
#include "Config.h"

/*
应用程序全局设置、数据。

*/

#define COMPANY_NAME _T("DYDOT")
#define APPLICATION_NAME _T("Erasure")

#define RUN_FLAG_ASADMINI 0x0010

class CLdApp
{
public:
	CLdApp();
	CLdString& GetAppPath();        //应用程序路径
	CLdString& GetAppDataPath();    //应用程序配置文件路径
	HINSTANCE GetInstance();
	DWORD GetMainThreadId();

	static BOOL Initialize(HINSTANCE hInstance);  //初始化，程序启动时调用
	static void MainThreadMessage(MSG& msg);      //消息循环过滤主线程消息  
	static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //发送消息到主线程，并执行回掉函数
	DWORD RunInvoker(TCHAR* Param, DWORD Flag, PVOID pContext);
	PVOID GetJobContext(DWORD pid);
	//CConfig* GetConfig();
private:
	HINSTANCE m_Instance;
	DWORD m_ThreadID;
	CLdString m_AppPath;
	CLdString m_AppDataPath;
	CLdMap<DWORD, PVOID> m_Job;
	//CConfig m_Config;
};

void DebugOutput(LPCTSTR pstrFormat, ...);

extern CLdApp* ThisApp;

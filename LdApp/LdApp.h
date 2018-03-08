#pragma once

/*
应用程序全局设置、数据。

*/

using namespace LeadowLib;

#define COMPANY_NAME _T("DYDOT")
#define APPLICATION_NAME _T("Erasure")

class CLdApp
{
public:
	CLdApp();
	~CLdApp();
	CLdString& GetAppPath();        //应用程序路径
	CLdString& GetAppDataPath();    //应用程序配置文件路径
	CLdString GetPluginPath();
	HINSTANCE GetInstance();
	DWORD GetMainThreadId();
	CLdString& GetAppName();

	static BOOL Initialize(HINSTANCE hInstance);  //初始化，程序启动时调用
	/*
	消息循环过滤主线程消息
	放在主消息循环中（while GetMessage）处理主线程消息。
	*/
	static void MainThreadMessage(MSG& msg);        
	BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //发送消息到主线程，并执行回掉函数


	//run 插件宿主进程
	DWORD RunPluginHost(TCHAR* Param, bool admin, HANDLE* hProcess);
private:
	HINSTANCE m_Instance;
	DWORD m_ThreadID;
	CLdString m_AppPath;
	CLdString m_AppDataPath;
	CLdString m_AppName;
};

void DebugOutput(LPCTSTR pstrFormat, ...);

extern CLdApp* ThisApp;

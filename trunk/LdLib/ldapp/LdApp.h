#pragma once
/*
应用程序全局设置、数据。

*/
#include "../classes/LdString.h"
#include "../utils/HandleUitls.h"
#include "../classes/LdMap.h"

namespace LeadowLib {

	class CLdApp
	{
	public:
		CLdApp();
		CLdString& GetInstallPath();    //应用程序安装路径
		CLdString& GetAppDataPath();    //应用程序配置文件路径
		HINSTANCE GetInstance();
		DWORD GetMainThreadId();

		//static CLdApp* ThisApp;         //

		static BOOL Initialize(HINSTANCE hInstance);  //初始化，程序启动时调用
		static void MainThreadMessage(MSG& msg);      //消息循环过滤主线程消息  
		static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //发送消息到主线程，并执行回掉函数
		DWORD RunInvoker(TCHAR* Param, DWORD Flag, PVOID pContext);
		PVOID GetJobContext(DWORD pid);
	private:
		HINSTANCE m_Instance;
		DWORD m_ThreadID;
		CLdString m_InstallPath;
		CLdString m_AppDataPath;
		CLdMap<DWORD, PVOID> m_Job;
	};

	void DebugOutput(LPCTSTR pstrFormat, ...);

	extern CLdApp* ThisApp;
};
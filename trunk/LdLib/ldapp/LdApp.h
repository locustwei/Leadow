#pragma once
/*
Ӧ�ó���ȫ�����á����ݡ�

*/
#include "../classes/LdString.h"
#include "../utils/HandleUitls.h"
#include "../classes/LdMap.h"

namespace LeadowLib {

	class CLdApp
	{
	public:
		CLdApp();
		CLdString& GetInstallPath();    //Ӧ�ó���װ·��
		CLdString& GetAppDataPath();    //Ӧ�ó��������ļ�·��
		HINSTANCE GetInstance();
		DWORD GetMainThreadId();

		//static CLdApp* ThisApp;         //

		static BOOL Initialize(HINSTANCE hInstance);  //��ʼ������������ʱ����
		static void MainThreadMessage(MSG& msg);      //��Ϣѭ���������߳���Ϣ  
		static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //������Ϣ�����̣߳���ִ�лص�����
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
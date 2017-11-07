#pragma once
#include "Config.h"

/*
Ӧ�ó���ȫ�����á����ݡ�

*/

class CLdApp
{
public:
	CLdApp();
	CLdString& GetInstallPath();    //Ӧ�ó���װ·��
	CLdString& GetAppDataPath();    //Ӧ�ó��������ļ�·��
	HINSTANCE GetInstance();
	DWORD GetMainThreadId();

	static BOOL Initialize(HINSTANCE hInstance);  //��ʼ������������ʱ����
	static void MainThreadMessage(MSG& msg);      //��Ϣѭ���������߳���Ϣ  
	static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //������Ϣ�����̣߳���ִ�лص�����
	DWORD RunInvoker(TCHAR* Param, DWORD Flag, PVOID pContext);
	PVOID GetJobContext(DWORD pid);
	CConfig* GetConfig();
private:
	HINSTANCE m_Instance;
	DWORD m_ThreadID;
	CLdString m_InstallPath;
	CLdString m_AppDataPath;
	CLdMap<DWORD, PVOID> m_Job;
	CConfig m_Config;
};

void DebugOutput(LPCTSTR pstrFormat, ...);

extern CLdApp* ThisApp;
#pragma once

/*
Ӧ�ó���ȫ�����á����ݡ�

*/

using namespace LeadowLib;

#define COMPANY_NAME _T("DYDOT")
#define APPLICATION_NAME _T("Erasure")

class CLdApp
{
public:
	CLdApp();
	~CLdApp();
	CLdString& GetAppPath();        //Ӧ�ó���·��
	CLdString& GetAppDataPath();    //Ӧ�ó��������ļ�·��
	HINSTANCE GetInstance();
	DWORD GetMainThreadId();

	static BOOL Initialize(HINSTANCE hInstance);  //��ʼ������������ʱ����
	/*
	��Ϣѭ���������߳���Ϣ
	��������Ϣѭ���У�while GetMessage���������߳���Ϣ��
	*/
	static void MainThreadMessage(MSG& msg);        
	BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //������Ϣ�����̣߳���ִ�лص�����
	//run �����������
	DWORD RunPluginHost(TCHAR* Param, bool admin, HANDLE* hProcess);
	//PVOID GetJobContext(DWORD pid);
	//CConfig* GetConfig();
private:
	HINSTANCE m_Instance;
	DWORD m_ThreadID;
	CLdString m_AppPath;
	CLdString m_AppDataPath;
	//CLdMap<DWORD, PVOID> m_Job;
	//CConfig m_Config;
};

void DebugOutput(LPCTSTR pstrFormat, ...);

extern CLdApp* ThisApp;

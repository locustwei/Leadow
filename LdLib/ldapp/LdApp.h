#pragma once
/*
Ӧ�ó���ȫ�����á����ݡ�

*/
#include "../classes/LdString.h"
#include "../utils/HandleUitls.h"

namespace LeadowLib {

	class CLdApp
	{
	public:
		CLdApp();
		CLdString& GetInstallPath();    //Ӧ�ó���װ·��
		CLdString& GetAppDataPath();    //Ӧ�ó��������ļ�·��
		HINSTANCE GetInstance();
		DWORD GetMainThreadId();

		static CLdApp* ThisApp;         //

		static BOOL Initialize(HINSTANCE hInstance);  //��ʼ������������ʱ����
		static void MainThreadMessage(MSG& msg);      //��Ϣѭ���������߳���Ϣ  
		static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param); //������Ϣ�����̣߳���ִ�лص�����
		BOOL RunInvoker(TCHAR* Param, DWORD Flag, UINT nPort);
	private:
		HINSTANCE m_Instance;
		DWORD m_ThreadID;
		CLdString m_InstallPath;
		CLdString m_AppDataPath;
	};

	void DebugOutput(LPCTSTR pstrFormat, ...);

};
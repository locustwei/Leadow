#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
*/

#include <LdCommunication.h>
#include <LdPlugin.h>

class CErasureImpl
	: public IPluginInterface
	, public ICommunicationListen
{
public:
	CErasureImpl();
	~CErasureImpl() override;
	HMODULE GetModuleHandle();
protected: //IPluginInterface
	CFramNotifyPump* CreateUI() override;
	DWORD InitCommunicate() override;
protected: //ICommunicationListen
	bool OnCreate() override;
	void OnTerminate(DWORD exitcode) override;
	void OnCommand(WORD id, TCHAR* ProcessName, PVOID data, WORD nSize) override;
private:
	HMODULE m_hModule;
	CEreaserThrads m_EraseThread;
	CLdArray<CVirtualFile*> m_Files;
	CLdCommunication* m_Comm;
	DWORD SetFolderFilesData(CVirtualFile * pFile);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD EraseFile(CDynObject& Param);
	DWORD EraseVolume(CDynObject& Param);
	DWORD FileAnalysis(TCHAR* Param, TCHAR* progress);
	DWORD AnaVolume(CDynObject& Param);
	
	INT_PTR FileAnalyThread(PVOID pData, UINT_PTR Param);   //�����ļ������̡߳�
};

extern CErasureImpl* ErasureImpl;

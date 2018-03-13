#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
*/

#include "IPC/LdCommunication.h"

class CFileEraserComm;

class CErasureImpl
	: public IPluginInterface
	, public ICommunicationListen
{
public:
	CErasureImpl();
	~CErasureImpl();
	HMODULE GetModuleHandle();
protected: //IPluginInterface
	CFramNotifyPump* CreateUI() override;
	DWORD InitCommunicate() override;
protected: //ICommunicationListen
	bool OnCreate() override;
	void OnTerminate(DWORD exitcode) override;
	void OnCommand(WORD id, PVOID data, WORD nSize) override;
private:
	HMODULE m_hModule;
	CEreaserThrads m_EraseThread;
	CLdArray<CVirtualFile*> m_Files;
	CFileEraserComm* m_Comm;
	DWORD SetFolderFilesData(CVirtualFile * pFile);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD EraseFile(CDynObject& Param);
	DWORD EraseVolume(CDynObject& Param);
	DWORD FileAnalysis(CDynObject Param);
	DWORD AnaVolume(CDynObject& Param);
	
	INT_PTR FileAnalyThread(PVOID pData, UINT_PTR Param);   //�����ļ������̡߳�
};

extern CErasureImpl* ErasureImpl;

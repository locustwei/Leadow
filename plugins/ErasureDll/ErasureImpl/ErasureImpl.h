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
	void OnCommand(WORD id, CDynObject& Param) override;
private:
	HMODULE m_hModule;
	CEreaserThrads m_EraseThread;
	CLdArray<CVirtualFile*> m_Files;
	CLdCommunication* m_Comm;
	DWORD SetFolderFilesData(CVirtualFile * pFile);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD EraseFile(CDynObject& Param);
	DWORD EraseVolume(CDynObject& Param);
	DWORD FileAnalysis(CDynObject& Param);
	DWORD AnaVolume(CDynObject& Param);
	
};

extern CErasureImpl* ErasureImpl;

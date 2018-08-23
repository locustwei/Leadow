#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
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

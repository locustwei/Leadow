#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
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
	DWORD EraseFile(CDynObject& Param, IEraserListen* callback);
	DWORD EraseVolume(CDynObject& Param, IEraserListen* callback);
	DWORD FileAnalysis(CDynObject Param, IEraserListen* callback);
	DWORD AnaVolume(CDynObject& Param, IEraserListen* callback);
};

extern CErasureImpl* ErasureImpl;

#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
*/

#include <LdCommunication.h>
#include <LdPlugin.h>
#include "ErasureThread.h"

class CErasureImpl
	: public IPluginInterface
	, public ICommunicationListen
	, IEraserListen
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
protected: //IEraserListen
	virtual bool EraserReprotStatus(TCHAR* FileName, TCHAR*, E_THREAD_OPTION op, LD_ERROR_CODE, INT_PTR Value) override;
private:
	HMODULE m_hModule;
	//CEreaserThrads m_EraseThread;
	//CLdArray<CVirtualFile*> m_Files;
	CLdCommunication* m_Comm;
	//DWORD SetFolderFilesData(CVirtualFile * pFile);
	//void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
	DWORD EraseFiles(CDynObject& Param);
	DWORD EraseVolume(CDynObject& Param);
	DWORD FileAnalysis(CDynObject& Param);
	DWORD VolumeAnalysis(CDynObject& Param);
	
};

extern CErasureImpl* ErasureImpl;

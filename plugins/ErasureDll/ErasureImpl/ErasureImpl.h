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
	
	INT_PTR FileAnalyThread(PVOID pData, UINT_PTR Param);   //单个文件分析线程。
};

extern CErasureImpl* ErasureImpl;

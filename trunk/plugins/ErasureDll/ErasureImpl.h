#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
*/

#define PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262E")

class CErasureImpl
	:public IErasure
{
private:
	friend BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/);
public:
	CErasureImpl();
	~CErasureImpl();
	HMODULE GetModuleHandle();
protected:
	DWORD EraseFile(CDynObject& Param, IEraserListen* callback) override;
	DWORD EraseVolume(CDynObject& Param, IEraserListen* callback) override;
	DWORD AnaFile(CDynObject& Param, IEraserListen* callback) override;
	DWORD AnaVolume(CDynObject& Param, IEraserListen* callback) override;
	CFramNotifyPump* CreateUI() override;
//	TCHAR* GetPropertys(PLUGIN_PROPERTYS nproperty) override;
private:
	HMODULE m_hModule;
	CEreaserThrads m_EraseThread;
	CLdArray<CVirtualFile*> m_Files;
	DWORD SetFolderFilesData(CVirtualFile * pFile);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
};

extern CErasureImpl* ErasureImpl;

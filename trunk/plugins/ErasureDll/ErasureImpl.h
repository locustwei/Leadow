#pragma once

/*
动态库导出接口实现，并动态库全局设置和变量。(ThisLibrary 变量当前动态库的全局数据）
*/

class CErasureImpl
	:public IPluginInterface
{
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
private:
	HMODULE m_hModule;
	CEreaserThrads m_EraseThread;
	CLdArray<CVirtualFile*> m_Files;
	DWORD SetFolderFilesData(CVirtualFile * pFile);
	void FreeEraseFiles(CLdArray<CVirtualFile*>* files);
};

extern CErasureImpl* ErasureImpl;

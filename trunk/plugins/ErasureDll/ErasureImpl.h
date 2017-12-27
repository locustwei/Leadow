#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
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

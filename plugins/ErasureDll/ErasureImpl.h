#pragma once

/*
��̬�⵼���ӿ�ʵ�֣�����̬��ȫ�����úͱ�����(ThisLibrary ������ǰ��̬���ȫ�����ݣ�
*/

#include "IPC/LdCommunication.h"

class CFileEraserComm;

class CErasureImpl
	: public IErasure
	, public ICommunicationListen
{
private:
	friend BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/);
public:
	CErasureImpl();
	~CErasureImpl();
	HMODULE GetModuleHandle();
protected: //IErasure
	DWORD EraseFile(CDynObject& Param, IEraserListen* callback) override;
	DWORD EraseVolume(CDynObject& Param, IEraserListen* callback) override;
	DWORD AnaFile(CDynObject& Param, IEraserListen* callback) override;
	DWORD AnaVolume(CDynObject& Param, IEraserListen* callback) override;
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
};

extern CErasureImpl* ErasureImpl;

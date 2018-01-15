#pragma once
#include "LdCommunication.h"

class CFileEraserComm
	:public CLdCommunication
{
public:
	CFileEraserComm(ICommunicationListen* listen);
	CFileEraserComm(ICommunicationListen* listen, TCHAR* sharedata);
	~CFileEraserComm();

	DWORD LoadHost() override;

	DWORD ExecuteFileAnalysis(CLdArray<TCHAR*>* files);
protected:
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override;
};


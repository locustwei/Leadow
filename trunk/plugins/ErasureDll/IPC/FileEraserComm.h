#pragma once

#include "LdCommunication.h"
#include "..\define.h"

class CFileEraserComm
	:public CLdCommunication
{
public:
	CFileEraserComm(ICommunicationListen* listen);
	~CFileEraserComm();

	DWORD Connect();
	DWORD ExecuteFileAnalysis(CLdArray<CLdString>* files);
	void SendFileAnalyResult(TCHAR* FileName, PTEST_FILE_RESULT result);
protected:
};


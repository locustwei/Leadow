#pragma once

#include "LdCommunication.h"

class CFileEraserComm
	:public CLdCommunication
{
public:
	CFileEraserComm(ICommunicationListen* listen);
	~CFileEraserComm();

	DWORD Connect();
	DWORD ExecuteFileAnalysis(CLdArray<CLdString>* files);
	void AnalFile(TCHAR* file_name);
protected:
};


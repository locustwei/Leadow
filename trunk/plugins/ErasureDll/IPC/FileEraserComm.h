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
protected:
};


#pragma once

#include <LdCommunication.h>
#include "..\define.h"

class CUIComm
	:public CLdCommunication
{
public:
	CUIComm(ICommunicationListen* listen);
	~CUIComm();

	DWORD Connect();
	DWORD ExecuteFileAnalysis(CLdArray<CLdString>* files);
	void SendFileAnalyResult(TCHAR* FileName, PTEST_FILE_RESULT result);
protected:
};


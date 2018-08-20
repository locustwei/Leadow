#pragma once

#include <LdCommunication.h>
#include "..\define.h"

class CUIComm
	:public CLdCommunication
	, ICommunicationListen
{
public:
	CUIComm();
	~CUIComm() override;

	DWORD Connect();
	DWORD ExecuteFileAnalysis(CLdArray<CLdString>* files);
	void SendFileAnalyResult(TCHAR* FileName, PTEST_FILE_RESULT result);


protected:
	virtual bool OnCreate() override;


	virtual void OnTerminate(DWORD exitcode) override;


	virtual void OnCommand(WORD id, TCHAR* ProcessName, PVOID Param, WORD nParamSize) override;

};


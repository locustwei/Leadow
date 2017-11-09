#pragma once
#include "LdProcessCommunication.h"
#include "../LdApp/LdStructs.h"

class CEraserCComm:
	//public IEraserListen
	public CLdProcessCommunication
{
public:
	CEraserCComm();
	~CEraserCComm();

	BOOL Connect();
	BOOL SendEraseStatus(TCHAR* fileName, E_THREAD_OPTION op, DWORD value);
};


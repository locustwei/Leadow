#pragma once
#include "LdProcessCommunication.h"
#include "../LdFileEraser/ErasureLibrary.h"

class CEraserCComm:
	//public IEraserThreadCallback
	public CLdProcessCommunication
{
public:
	CEraserCComm();
	~CEraserCComm();

	BOOL Connect();
	BOOL SendEraseStatus(TCHAR* fileName, E_THREAD_OPTION op, DWORD value);
protected:

private:
};


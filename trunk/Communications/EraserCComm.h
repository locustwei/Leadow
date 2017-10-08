#pragma once
#include "LdProcessCommunication.h"
#include "../LdApp/LdStructs.h"

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
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
	void SetContext(PVOID) override;

private:
};


#pragma once
#include "LdProcessCommunication.h"
#include "../LdApp/LdStructs.h"

typedef struct COMM_ERASE_DATA
{
	E_THREAD_OPTION op;
	DWORD dwValue;
	TCHAR FileName[1];
}*PCOMM_ERASE_DATA;

class CEraserSComm
	:public CLdProcessCommunication
{
public:
	CEraserSComm();
	CEraserSComm(SOCKET s);
	~CEraserSComm();
	void SetContext(PVOID pContext) override;
	BOOL SendEraseStatus(E_THREAD_OPTION op, DWORD value);
protected:
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
private:
	//DWORD m_PID;
	IEraserListen* m_callback;
};


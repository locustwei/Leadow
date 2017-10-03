#pragma once
#include "LdProcessCommunication.h"
#include "../LdFileEraser/ErasureLibrary.h"

typedef struct COMM_ERASE_DATA
{
	E_THREAD_OPTION op;
	DWORD dwValue;
	TCHAR FileName[1];
}*PCOMM_ERASE_DATA;

class CEraserSComm
	:public CLdProcessCommunication
{
protected:
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
public:
	CEraserSComm();
	CEraserSComm(SOCKET s);
	~CEraserSComm();
};


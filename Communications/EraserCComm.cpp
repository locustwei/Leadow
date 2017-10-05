#include "stdafx.h"
#include  "EraserCComm.h"
#include  "EraserSComm.h"
#include <tchar.h>

CEraserCComm::CEraserCComm()
{
	
}

CEraserCComm::~CEraserCComm()
{

}

BOOL CEraserCComm::Connect()
{
	if (CLdProcessCommunication::Connect() != 0)
		return false;
	if (!SendData(LFI_EARSE_FILE, nullptr, 0))
		return false;
	int len = Recv();
	if (len < sizeof(COMMUINCATION_DATA))
		return false;
	PCOMMUINCATION_DATA precv = (PCOMMUINCATION_DATA)GetRecvData()->data;
	if (precv->fId != LFI_EARSE_FILE)
		return false;
	return StartSelectThread();
}

BOOL CEraserCComm::SendEraseStatus(TCHAR* fileName, E_THREAD_OPTION op, DWORD value)
{
	int len = sizeof(COMM_ERASE_DATA);
	if (fileName)
		len += _tcslen(fileName) * sizeof(TCHAR);
	PCOMM_ERASE_DATA pCommData = (PCOMM_ERASE_DATA)new BYTE[len];
	ZeroMemory(pCommData, len);
	pCommData->op = op;
	pCommData->dwValue = value;
	if (fileName)
		_tcscpy(pCommData->FileName, fileName);
	BOOL result = SendData(LFI_EARSE_FILE, pCommData, len);
	delete pCommData;
	return result;
}

void CEraserCComm::OnClosed(CLdSocket*)
{
	
}

void CEraserCComm::OnError(CLdSocket*, int)
{

}

void CEraserCComm::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{
}

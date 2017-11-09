#include "stdafx.h"
#include "EraserSComm.h"
#include "Communications.h"

void CEraserSComm::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{
	PCOMMUINCATION_DATA data = (PCOMMUINCATION_DATA)pData;
	PCOMM_ERASE_DATA pEraseData = (PCOMM_ERASE_DATA)data->Data;

	DebugOutput(L"RecvEraseStatus %d, %3d %s\n", pEraseData->op, pEraseData->dwValue, pEraseData->FileName);

	if (m_callback)
	{
		if (!m_callback->EraserReprotStatus(pEraseData->FileName, pEraseData->op, pEraseData->dwValue))
			SendEraseStatus(eto_abort, 0);
	}
}

BOOL CEraserSComm::SendEraseStatus(E_THREAD_OPTION op, DWORD value)
{
	int len = sizeof(COMM_ERASE_DATA);
	PCOMM_ERASE_DATA pCommData = (PCOMM_ERASE_DATA)new BYTE[len];
	ZeroMemory(pCommData, len);
	pCommData->op = op;
	pCommData->dwValue = value;
	BOOL result = SendData(LFI_EARSE_FILE, pCommData, len);
	delete pCommData;

	return result;
}

CEraserSComm::CEraserSComm(): m_callback(nullptr)
{

}

CEraserSComm::CEraserSComm(SOCKET s)
{
	m_Socket = s;
	m_Closed = m_Socket == INVALID_SOCKET;
	if(m_Closed)
	{
		if(m_Listner)
			static_cast<IClientListener*>(m_Listner)->OnConnected(this);
	}
}

CEraserSComm::~CEraserSComm()
{
}

void CEraserSComm::SetContext(PVOID pContext)
{
	m_callback = (IEraserListen*)pContext;
}

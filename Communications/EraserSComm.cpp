#include "stdafx.h"
#include "EraserSComm.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "Communications.h"

void CEraserSComm::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{
	PCOMMUINCATION_DATA data = (PCOMMUINCATION_DATA)pData;
	PCOMM_ERASE_DATA pEraseData = (PCOMM_ERASE_DATA)data->Data;
	switch(pEraseData->op)
	{
	case eto_start: break;
	case eto_begin: break;
	case eto_completed: break;
	case eto_progress: break;
	case eto_freespace: break;
	case eto_track: break;
	case eto_finished: break;
	case eto_analy: break;
	case eto_analied: break;
	default: break;
	}
	m_callback->EraserThreadCallback(pEraseData->FileName, pEraseData->op, pEraseData->dwValue);
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
	m_callback = (IEraserThreadCallback*)pContext;
}

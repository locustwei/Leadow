#include "stdafx.h"
#include "MainCommunication.h"
#include "LdProcessCommunication.h"

#pragma pack(push, 1)

typedef struct RECV_DATA
{
	WORD Id;
	WORD nLength;
	BYTE Data[1];
}*PRECV_DATA;

#pragma pack(pop)

CMainCommunication::CMainCommunication()
	:CLdServerSocket()
	,m_Clients()
{
	SetListener(this);
	Listen();
}


CMainCommunication::~CMainCommunication()
{
}

void CMainCommunication::OnClosed(CLdSocket*)
{
}

void CMainCommunication::OnError(CLdSocket*, int)
{
}

void CMainCommunication::OnAccept(CLdServerSocket*, CLdClientSocket* pClient)
{
	pClient->Recv();
	PLDSOCKET_DATA pData = pClient->GetRecvData();
	if(!pData || pData->nSize == 0)
	{
		delete pClient;
		return;
	}
	PRECV_DATA precv = (PRECV_DATA)pData->data;
	switch (precv->Id)
	{
	case LFI_EARSE_FILE:
		pClient->SetListener(new CLdProcessCommunication());
		break;
	default:
		delete pClient;
		return;
	}

	pClient->SetListener(dynamic_cast<IClientListener *>(this));
	pClient->StartSelectThread();

	m_Clients.Add(pClient);
}

#include "stdafx.h"
#include "MainCommunication.h"

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
	SetListener(dynamic_cast<IServerListener *>(this));
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
	pClient->SetListener(dynamic_cast<IClientListener *>(this));
	m_Clients.Add(pClient);
}

void CMainCommunication::OnConnected(CLdClientSocket*)
{
}

void CMainCommunication::OnRecv(CLdClientSocket* pClient, PBYTE pData, WORD nLength)
{
	if(nLength < sizeof(RECV_DATA))
	{
		pClient->Send()
	}
}

#include "stdafx.h"
#include "MainCommunication.h"
#include "EraserSComm.h"

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

void CMainCommunication::OnAccept(CLdServerSocket*, SOCKET socket)
{
	BYTE buffer[100] = { 0 };
	int len = recv(socket, (char*)buffer, 100, 0);
	if (len < sizeof(LDSOCKET_DATA))
	{
		closesocket(socket);
		return;
	}
	PLDSOCKET_DATA pData = (PLDSOCKET_DATA)buffer;
	if(!pData || pData->nSize == 0)
	{
		closesocket(socket);
		return;
	}

	CLdProcessCommunication* pComm = nullptr;

	PCOMMUINCATION_DATA precv = (PCOMMUINCATION_DATA)pData->data;
	switch (precv->fId)
	{
	case LFI_EARSE_FILE:
		//pClient->SetListener(new CEraserSComm());
		pComm = new CEraserSComm(socket);
		pComm->SendData(LFI_EARSE_FILE, nullptr, 0);
		break;
	default:
		closesocket(socket);
		return;
	}
	if (pComm)
	{
		pComm->StartSelectThread();
		m_Clients.Add(pComm);
	}
}

void CMainCommunication::SetSocket(CLdSocket *)
{
}

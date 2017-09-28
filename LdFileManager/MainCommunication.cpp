#include "stdafx.h"
#include "MainCommunication.h"

void CServerListenerImpl::OnClosed(CLdSocket*)
{
}

void CServerListenerImpl::OnError(CLdSocket*, int)
{
}

void CServerListenerImpl::OnAccept(CLdServerSocket*, CLdClientSocket*)
{
}

CMainCommunication::CMainCommunication()
	:CLdServerSocket()
	,m_Listener()
{
	SetListener(&m_Listener);
	Listen();
}


CMainCommunication::~CMainCommunication()
{
}

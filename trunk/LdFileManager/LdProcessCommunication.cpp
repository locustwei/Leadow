#include "stdafx.h"
#include "LdProcessCommunication.h"



CLdProcessCommunication::CLdProcessCommunication(): m_Socket(nullptr)
{
}


CLdProcessCommunication::~CLdProcessCommunication()
{
}

void CLdProcessCommunication::OnClosed(CLdSocket*)
{
}

void CLdProcessCommunication::OnError(CLdSocket *, int)
{
}

void CLdProcessCommunication::OnConnected(CLdClientSocket *)
{
}

void CLdProcessCommunication::OnRecv(CLdClientSocket *, PBYTE pData, WORD nLength)
{
}

void CLdProcessCommunication::SetSocket(CLdSocket* socket)
{
	m_Socket = static_cast<CLdClientSocket*>(socket);
}

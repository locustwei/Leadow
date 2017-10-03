#include "stdafx.h"
#include "LdProcessCommunication.h"



CLdProcessCommunication::CLdProcessCommunication()
	//: m_Socket(nullptr)
{
	SetListener(this);
}

CLdProcessCommunication::~CLdProcessCommunication()
{
}

BOOL CLdProcessCommunication::SendData(DWORD id, PVOID pData, WORD Length)
{
	if (IsClosed())
		return false;
	PCOMMUINCATION_DATA data = (PCOMMUINCATION_DATA)new BYTE[Length + sizeof(COMMUINCATION_DATA)];
	data->cId = id;
	data->nLength = Length;
	if (pData && Length > 0)
	{
		MoveMemory(data->Data, pData, Length);
	}
	return Send(data, Length + sizeof(COMMUINCATION_DATA)) == 0;
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
	//m_Socket = static_cast<CLdClientSocket*>(socket);
}

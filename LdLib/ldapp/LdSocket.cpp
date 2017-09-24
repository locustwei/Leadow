#include "stdafx.h"
#include "LdSocket.h"

#pragma comment(lib, "ws2_32.lib")

namespace LeadowLib
{
#define RECV_BUFFER_LEN 1024

	CSocketBase::CSocketBase(): m_port(0), tag(0)
	{
		m_Socket = INVALID_SOCKET;
		m_Status = SS_NONE;
		lpRecvedBuffer = NULL;
		nRecvSize = 0;
		bClosed = true;

	}

	CSocketBase::CSocketBase(SOCKET s): m_port(0), tag(0)
	{
		m_Socket = s;
		m_Status = SS_NONE;
		lpRecvedBuffer = NULL;
		nRecvSize = 0;
		bClosed = true;
	}

	CSocketBase::~CSocketBase()
	{
		Close();
		if (lpRecvedBuffer)
			delete lpRecvedBuffer;
	}

	void CSocketBase::Close()
	{
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		bClosed = true;
	}

	int CSocketBase::Connect(LPCSTR szIp, int port)
	{
		if (m_Socket != INVALID_SOCKET)
			return SOCKET_ERROR;
		m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
			return WSAGetLastError();
		sockaddr_in address = { 0 };
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = inet_addr(szIp);
		address.sin_port = htons(port);

		if (connect(m_Socket, (const sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return WSAGetLastError();
		}
		return 0;
	}

	int CSocketBase::Send(PVOID buffer, int nSize)
	{
		if (m_Socket == INVALID_SOCKET || !buffer || nSize == 0)
			return SOCKET_ERROR;

		int nCount;
		do
		{
			nCount = send(m_Socket, (char*)buffer, nSize, 0);
			if (nCount == SOCKET_ERROR) {
				return WSAGetLastError();
			}
			else {
				buffer = (char*)buffer + nCount;
				nSize -= nCount;
			}

		} while (nSize > 0);
		return 0;
	}

	int CSocketBase::Recv()
	{
		int nBytes, nTotal = 0;;
		if (lpRecvedBuffer)
			ZeroMemory(lpRecvedBuffer, nRecvSize);
		do
		{
			char buffer[RECV_BUFFER_LEN] = { 0 };
			nBytes = recv(m_Socket, buffer, RECV_BUFFER_LEN, 0);
			if (nBytes > 0) {
				if (nTotal + nBytes > nRecvSize) {
					nRecvSize = nTotal + nBytes;
					lpRecvedBuffer = (PUCHAR)realloc(lpRecvedBuffer, nRecvSize);
				}
				CopyMemory(lpRecvedBuffer + nTotal, buffer, nBytes);
				nTotal += nBytes;
			}
			else
				return nBytes;

		} while (nBytes >= RECV_BUFFER_LEN);

		if (nBytes == SOCKET_ERROR)
			return SOCKET_ERROR;

		return nTotal;
	}

	CLdSocket::CLdSocket(void)
		:m_ClientSockets()
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		m_Listner = NULL;
	}


	CLdSocket::~CLdSocket(void)
	{
		for(int i=0; i<m_ClientSockets.GetCount(); i++)
		{        //删除客户端
			RemoveClient(m_ClientSockets[i]);
		}
		m_ClientSockets.Clear();

		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}

		m_Listner = NULL;
		m_Status = SS_NONE;
	}

	BOOL CLdSocket::Bind(int port)
	{
		if (m_Socket != INVALID_SOCKET)
			return FALSE;
		m_Socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_Socket == INVALID_SOCKET)
			return FALSE;
		sockaddr_in address = { 0 };
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(port);

		if (bind(m_Socket, (const sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		};

		m_Status = SS_BINDED;

		return TRUE;
	}

	BOOL CLdSocket::Listen(int port)
	{
		if (m_Socket != INVALID_SOCKET)
			return FALSE;
		m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
			return FALSE;
		sockaddr_in address = { 0 };
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = htonl(INADDR_ANY);
		address.sin_port = htons(port);

		if (bind(m_Socket, (const sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		};

		if (listen(m_Socket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		}

		if (!StartSelectThread()) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		}

		m_Status = SS_LISTENING;

		return TRUE;
	}

/*
	BOOL CLdSocket::ConnectTo(LPCSTR szIp, int port)
	{
		if (m_Socket != INVALID_SOCKET)
			return FALSE;
		m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
			return FALSE;
		sockaddr_in address = { 0 };
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = inet_addr(szIp);
		address.sin_port = htons(port);

		if (connect(m_Socket, (const sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		}

		u_long nNoBlock = 1;
		ioctlsocket(m_Socket, FIONBIO, &nNoBlock);

		if (!StartSelectThread()) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return FALSE;
		}

		m_Status = SS_CONNECTED;

		if (m_Listner)
			m_Listner->OnConnected(this);

		return TRUE;
	}
*/
/*

	DWORD WINAPI SocketSelectThreadProc(
		_In_ LPVOID lpParameter
	)
	{
		fd_set ReadSet, WriteSet, ExceptSet;
		CLdSocket* ldSocket = (CLdSocket*)lpParameter;
		while (true) {
			if (ldSocket->m_Socket == INVALID_SOCKET)
				break;

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);        //不处理send
			FD_ZERO(&ExceptSet);

			FD_SET(ldSocket->m_Socket, &ReadSet);
			FD_SET(ldSocket->m_Socket, &ExceptSet);

			//todo (线程同步问题）
			CSocketBase* pClient = ldSocket->GetClientHead();
			while (pClient) {
				if (!pClient->bClosed) {
					FD_SET(pClient->m_Socket, &ReadSet);
					FD_SET(pClient->m_Socket, &ExceptSet);
				}
				pClient = pClient->pNext;
			}

			if (select(0, &ReadSet, NULL, &ExceptSet, 0) > 0) {
				if (FD_ISSET(ldSocket->m_Socket, &ReadSet)) {
					ldSocket->DoRead();
				}

				if (FD_ISSET(ldSocket->m_Socket, &ExceptSet)) {
					ldSocket->DoClientExcept(ldSocket);
				}
				pClient = ldSocket->GetClientHead();
				while (pClient) {
					if (!pClient->bClosed) {
						if (FD_ISSET(pClient->m_Socket, &ReadSet)) {
							ldSocket->DoClientRead(pClient);
						}

						if (FD_ISSET(ldSocket->m_Socket, &ExceptSet)) {
							ldSocket->DoClientExcept(pClient);
						}
					}
					pClient = pClient->pNext;
				}

				pClient = ldSocket->GetClientHead();
				while (pClient) {  //删除已经断开连接的客户端
					if (pClient->bClosed) {
						CSocketBase* pTmp = pClient->pNext;
						ldSocket->RemoveClient(pClient);
						pClient = pTmp;
					}
					else
						pClient = pClient->pNext;
				}
			}


		}
		ldSocket->m_hSelectThread = NULL;
		return 0;
	}

*/

	BOOL CLdSocket::StartSelectThread()
	{
		CThread* thread = new CThread(this);
		return thread->Start(0) != INVALID_HANDLE_VALUE;
	}

	CSocketBase* CLdSocket::AddClient(SOCKET s)
	{
		if (s == INVALID_SOCKET)
			return nullptr;

		u_long nNoBlock = 1;
		ioctlsocket(s, FIONBIO, &nNoBlock);

		CSocketBase* pClient = new CSocketBase(s);
		m_ClientSockets.Add(pClient);
		return pClient;
	}

	void CLdSocket::DoRead()
	{
		if (m_Status == SS_LISTENING)
			return DoAccept();
		else
			return DoClientRead(this);
	}

	void CLdSocket::DoClientRead(CSocketBase* pClient)
	{
		int n = pClient->Recv();
		if (n == SOCKET_ERROR) {
			DoClientExcept(pClient);
		}
		else if (n == 0)
			DoClientClosed(pClient);
		else if (m_Listner)
			m_Listner->OnRecv(pClient);
	}

	void CLdSocket::DoClientExcept(CSocketBase* pClient)
	{
		int err = WSAGetLastError();
		switch (err) {
		case WSAECONNRESET:
			DoClientClosed(pClient);
			break;
		default:
			if (m_Listner) {
				m_Listner->OnError(pClient, err);
			};
			break;
		}
	}

	void CLdSocket::DoAccept()
	{
		sockaddr_in ClientAddress;
		int nClientLength = sizeof(ClientAddress);

		SOCKET Socket = accept(m_Socket, (sockaddr*)&ClientAddress, &nClientLength);

		if (INVALID_SOCKET == Socket) {
			DoClientExcept(this);
			return;
		}
		CSocketBase* pClient = AddClient(Socket);
		if (m_Listner && pClient)
			m_Listner->OnAccept(pClient);
	}

/*
	int CLdSocket::RecvData(CSocketBase* pClient)
	{
		//todo 线程同步问题

		int nBytes, nTotal = 0;;
		if (pClient->lpRecvedBuffer)
			ZeroMemory(pClient->lpRecvedBuffer, pClient->nRecvSize);
		do
		{
			char buffer[RECV_BUFFER_LEN] = { 0 };
			nBytes = recv(pClient->m_Socket, buffer, RECV_BUFFER_LEN, 0);
			if (nBytes > 0) {
				if (nTotal + nBytes > pClient->nRecvSize) {
					pClient->lpRecvedBuffer = (char*)realloc(pClient->lpRecvedBuffer, nTotal + nBytes);
				}
				CopyMemory(pClient->lpRecvedBuffer + nTotal, buffer, nBytes);
				nTotal += nBytes;
			}
		} while (nBytes >= RECV_BUFFER_LEN);

		pClient->nRecvSize = nTotal;

		if (nBytes == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (nTotal == 0)
			return 0;
		return nTotal;
	}
*/

	void CLdSocket::DoClientClosed(CSocketBase* pClient)
	{
		pClient->Close();
		if (m_Listner)
			m_Listner->OnClosed(pClient);
	}

	void CLdSocket::SetListener(ISocketListener* listener)
	{
		m_Listner = listener;
	}

	void CLdSocket::RemoveClient(CSocketBase* pClient)
	{
		if (!pClient)
			return;
		m_ClientSockets.Remove(pClient);		
		delete pClient;
	}

	void CLdSocket::ThreadBody(CThread * Sender, UINT_PTR Param)
	{
		fd_set ReadSet, WriteSet, ExceptSet;

		while (true) {
			if (m_Socket == INVALID_SOCKET)
				break;

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);        //不处理send
			FD_ZERO(&ExceptSet);

			FD_SET(m_Socket, &ReadSet);
			FD_SET(m_Socket, &ExceptSet);

			//todo (线程同步问题）
			//CSocketBase* pClient = ldSocket->GetClientHead();
			//while (pClient) 
			for(int i=0; i<m_ClientSockets.GetCount(); i++)
			{
				CSocketBase* pClient = m_ClientSockets[i];
				if (!pClient->bClosed)
				{
					FD_SET(pClient->m_Socket, &ReadSet);
					FD_SET(pClient->m_Socket, &ExceptSet);
				}
			}

			if (select(0, &ReadSet, NULL, &ExceptSet, 0) > 0) {
				if (FD_ISSET(m_Socket, &ReadSet)) {
					DoRead();
				}

				if (FD_ISSET(m_Socket, &ExceptSet)) {
					DoClientExcept(this);
				}
				for (int i = 0; i<m_ClientSockets.GetCount(); i++)
				{
					CSocketBase* pClient = m_ClientSockets[i];
					if (!pClient->bClosed) {
						if (FD_ISSET(pClient->m_Socket, &ReadSet)) {
							DoClientRead(pClient);
						}

						if (FD_ISSET(m_Socket, &ExceptSet)) {
							DoClientExcept(pClient);
						}
					}
				}

				for (int i = 0; i<m_ClientSockets.GetCount(); i++)
				{//删除已经断开连接的客户端
					CSocketBase* pClient = m_ClientSockets[i]; 
					if (pClient->bClosed) 
					{
						RemoveClient(pClient);
						m_ClientSockets.Delete(i);
					}
				}
			}


		}
	}

	void CLdSocket::OnThreadInit(CThread * Sender, UINT_PTR Param)
	{
	}

	void CLdSocket::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
	{
	}

	ISocketListener* CLdSocket::GetListener()
	{
		return m_Listner;
	}
}
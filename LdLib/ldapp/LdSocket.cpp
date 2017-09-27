#include "stdafx.h"
#include "LdSocket.h"

#pragma comment(lib, "ws2_32.lib")

namespace LeadowLib
{
#define RECV_BUFFER_LEN 1024

	CLdSocket::CLdSocket(): m_port(0), m_Listner(nullptr), m_tag(0)
	{
		m_Socket = INVALID_SOCKET;
		m_Closed = true;
	}

	CLdSocket::~CLdSocket()
	{
		Close();
		
	}

	bool CLdSocket::IsClosed() const
	{
		return m_Closed;
	}

	UINT_PTR CLdSocket::GetTag() const
	{
		return m_tag;
	}

	void CLdSocket::SetTag(UINT_PTR value)
	{
		m_tag = value;
	}

	void CLdSocket::SetListener(ISocketListener * listener)
	{
		m_Listner = listener;
	}

	ISocketListener * CLdSocket::GetListener()
	{
		return m_Listner;
	}

	void CLdSocket::Close()
	{
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		m_Closed = true;
	}

	void CLdSocket::DoExcept()
	{
		int err = WSAGetLastError();
		switch (err) {
		case WSAECONNRESET:
			DoClose();
			break;
		default:
			if (m_Listner) {
				m_Listner->OnError(this, err);
			};
			break;
		}
	}

	void CLdSocket::DoClose()
	{
		Close();
		if (m_Listner)
			m_Listner->OnClosed(this);

	}

	CLdClientSocket::CLdClientSocket(void)
		: CLdSocket()
		, m_Buffer(nullptr)
		, m_RecvSize(0)
	{
		
	}


	CLdClientSocket::~CLdClientSocket(void)
	{
		if (m_Socket != INVALID_SOCKET) {
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}

		m_Listner = NULL;
	}

	int CLdClientSocket::Connect(LPCSTR szIp, int port)
	{
		if (m_Socket != INVALID_SOCKET)
			return SOCKET_ERROR;
		m_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_Socket == INVALID_SOCKET)
			return WSAGetLastError();
		int result = 0;
		sockaddr_in address = { 0 };
		address.sin_family = PF_INET;
		address.sin_addr.s_addr = inet_addr(szIp);
		address.sin_port = htons(port);

		if (connect(m_Socket, (const sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
			Close();
			result = WSAGetLastError();
		}
		
		if(result==0)
		{
			if (!StartSelectThread())
			{
				result = GetLastError();
				Close();
			}
		}
		return result;
	}

	int CLdClientSocket::Send(PVOID buffer, WORD nSize)
	{
		if (m_Socket == INVALID_SOCKET || !buffer || nSize == 0)
			return SOCKET_ERROR;

		PBYTE p = new BYTE[sizeof(WORD) + nSize];
		((PLDSOCKET_DATA)p)->nSize = nSize;
		memcpy(((PLDSOCKET_DATA)p)->data, buffer, nSize);
		nSize += sizeof(WORD);
		int nCount;
		PBYTE p1 = p;
		do
		{
			nCount = send(m_Socket, (char*)p1, nSize, 0);
			if (nCount == SOCKET_ERROR) {
				return WSAGetLastError();
			}
			else {
				p1 = p1 + nCount;
				nSize -= nCount;
			}

		} while (nSize > 0);

		delete p;

		return 0;
	}

	int CLdClientSocket::Recv()
	{
		int nBytes, nTotal = 0;;
		if (m_Buffer)
			ZeroMemory(m_Buffer, m_RecvSize);
		do
		{
			char buffer[RECV_BUFFER_LEN] = { 0 };
			nBytes = recv(m_Socket, buffer, RECV_BUFFER_LEN, 0);
			if (nBytes > 0) {
				if (nTotal + nBytes > m_RecvSize) {
					m_RecvSize = nTotal + nBytes;
					m_Buffer = (PUCHAR)realloc(m_Buffer, m_RecvSize);
				}
				CopyMemory(m_Buffer + nTotal, buffer, nBytes);
				nTotal += nBytes;
			}
			else
				return nBytes;

		} while (nBytes >= RECV_BUFFER_LEN);

		if (nBytes == SOCKET_ERROR)
			return SOCKET_ERROR;

		return nTotal;
	}

	PLDSOCKET_DATA CLdClientSocket::GetRecvData() const
	{
		return (PLDSOCKET_DATA)m_Buffer;
	}

	int CLdClientSocket::GetRecvSize() const
	{
		return m_RecvSize;
	}

	CLdClientSocket::CLdClientSocket(SOCKET s)
		:CLdSocket()
		, m_Buffer(nullptr)
		, m_RecvSize(0)
	{
		m_Socket = s;
	}

	/*
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
	
			return TRUE;
		}
	*/


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
			CLdSocket* pClient = ldSocket->GetClientHead();
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
						CLdSocket* pTmp = pClient->pNext;
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

	BOOL CLdClientSocket::StartSelectThread()
	{
		CThread* thread = new CThread(this);
		return thread->Start(0) != INVALID_HANDLE_VALUE;
	}

	void CLdClientSocket::DoRead()
	{
		int n = Recv();
		if (n == SOCKET_ERROR) {
			DoExcept();
		}
		else if (n == 0)
			DoClose();
		else if (m_Listner)
		{
			PLDSOCKET_DATA p = GetRecvData();
			int i = 0;
			while (i<GetRecvSize())
			{
				m_Listner->OnRecv(this, p->data, p->nSize);
				p = (PLDSOCKET_DATA)((char*)p->data + p->nSize);
				i += p->nSize;
			}

		}
	}

/*
	void CLdSocket::DoClientRead(CLdSocket* pClient)
	{
		int n = pClient->Recv();
		if (n == SOCKET_ERROR) {
			DoClientExcept(pClient);
		}
		else if (n == 0)
			DoClientClosed(pClient);
		else if (m_Listner)
		{
			PLDSOCKET_DATA p = pClient->GetRecvData();
			int i = 0;
			while (i<pClient->GetRecvSize())
			{
				m_Listner->OnRecv(pClient, p->data, p->nSize);
				p = (PLDSOCKET_DATA)((char*)p->data + p->nSize);
				i += p->nSize;
			}

		}
	}
*/

/*
	int CLdSocket::RecvData(CLdSocket* pClient)
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

	void CLdClientSocket::ThreadBody(CThread * Sender, UINT_PTR Param)
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

			if (select(0, &ReadSet, NULL, &ExceptSet, 0) > 0) {
				if (FD_ISSET(m_Socket, &ReadSet)) {
					DoRead();
				}

				if (FD_ISSET(m_Socket, &ExceptSet)) {
					DoExcept();
				}
			}
		}
	}

	void CLdClientSocket::OnThreadInit(CThread * Sender, UINT_PTR Param)
	{
	}

	void CLdClientSocket::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
	{
	}

	CLdServerSocket::CLdServerSocket()
		:m_ClientSockets()
	{
	}

	CLdServerSocket::~CLdServerSocket()
	{
		for (int i = 0; i<m_ClientSockets.GetCount(); i++)
		{        //删除客户端
			RemoveClient(m_ClientSockets[i]);
		}
		m_ClientSockets.Clear();
	}

	BOOL CLdServerSocket::Listen(int port)
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

		return TRUE;
	}

	int CLdServerSocket::GetClientCount()
	{
		return m_ClientSockets.GetCount();
	}

	CLdClientSocket* CLdServerSocket::GetClient(int idx)
	{
		return m_ClientSockets[idx];
	}

	BOOL CLdServerSocket::StartSelectThread()
	{
		CThread* thread = new CThread(this);
		return thread->Start(0) != INVALID_HANDLE_VALUE;
	}

	void CLdServerSocket::DoAccept()
	{
		sockaddr_in ClientAddress;
		int nClientLength = sizeof(ClientAddress);

		SOCKET Socket = accept(m_Socket, (sockaddr*)&ClientAddress, &nClientLength);

		if (INVALID_SOCKET == Socket) {
			return;
		}
		CLdClientSocket* pClient = AddClient(Socket);
		if (m_Listner && pClient)
			m_Listner->OnAccept(pClient);
	}

	void CLdServerSocket::RemoveClient(CLdClientSocket* pClient)
	{
		if (!pClient)
			return;
		m_ClientSockets.Remove(pClient);
		delete pClient;

	}

	void CLdServerSocket::ThreadBody(CThread* Sender, UINT_PTR Param)
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

			for (int i = 0; i<m_ClientSockets.GetCount(); i++)
			{
				CLdClientSocket* pClient = m_ClientSockets[i];
				if (!pClient->m_Closed)
				{
					FD_SET(pClient->m_Socket, &ReadSet);
					FD_SET(pClient->m_Socket, &ExceptSet);
				}
			}

			if (select(0, &ReadSet, NULL, &ExceptSet, 0) > 0) {
				if (FD_ISSET(m_Socket, &ReadSet)) {
					DoAccept();
				}

				if (FD_ISSET(m_Socket, &ExceptSet)) {
					DoExcept();
				}
				for (int i = 0; i<m_ClientSockets.GetCount(); i++)
				{
					CLdClientSocket* pClient = m_ClientSockets[i];
					if (!pClient->m_Closed) {
						if (FD_ISSET(pClient->m_Socket, &ReadSet)) {
							pClient->DoRead();
						}

						if (FD_ISSET(m_Socket, &ExceptSet)) {
							pClient->DoExcept();
						}
					}
				}

				for (int i = 0; i<m_ClientSockets.GetCount(); i++)
				{//删除已经断开连接的客户端
					CLdClientSocket* pClient = m_ClientSockets[i];
					if (pClient->m_Closed)
					{
						RemoveClient(pClient);
						m_ClientSockets.Delete(i);
					}
				}
			}
		}
	}

	void CLdServerSocket::OnThreadInit(CThread* Sender, UINT_PTR Param)
	{
	}

	void CLdServerSocket::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
	{
	}

	CLdClientSocket* CLdServerSocket::AddClient(SOCKET s)
	{
		if (s == INVALID_SOCKET)
			return nullptr;

		u_long nNoBlock = 1;
		ioctlsocket(s, FIONBIO, &nNoBlock);

		CLdClientSocket* pClient = new CLdClientSocket(s);
		m_ClientSockets.Add(pClient);
		return pClient;
	}
}

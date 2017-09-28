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

	SOCKET CLdSocket::GetHandle()
	{
		return m_Socket;
	}

	int CLdSocket::InitSocketDll()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD(2, 2);
		return WSAStartup(wVersionRequested, &wsaData);
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
		, m_hThread(INVALID_HANDLE_VALUE)
		, m_Buffer(nullptr)
		, m_RecvSize(0)
	{
		
	}


	CLdClientSocket::~CLdClientSocket(void)
	{
		Close();
		if(m_hThread != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(m_hThread, INFINITE);
			m_hThread = INVALID_HANDLE_VALUE;
		}
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

			m_addr = address.sin_addr;
			m_port = address.sin_port;
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
		{
			return SOCKET_ERROR;
		}
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
		, m_hThread(INVALID_HANDLE_VALUE)
		, m_Buffer(nullptr)
		, m_RecvSize(0)
	{
		m_Socket = s;
		m_Closed = m_Socket == INVALID_SOCKET;
	}

	BOOL CLdClientSocket::StartSelectThread()
	{
		CThread* thread = new CThread(this);
		m_hThread = thread->Start(0);
		return m_hThread != INVALID_HANDLE_VALUE;
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
				static_cast<IClientListener*>(m_Listner)->OnRecv(this, p->data, p->nSize);
				p = (PLDSOCKET_DATA)((char*)p->data + p->nSize);
				i += p->nSize;
			}

		}
	}

	void CLdClientSocket::ThreadBody(CThread * Sender, UINT_PTR Param)
	{
		fd_set ReadSet, WriteSet, ExceptSet;

		while (m_Socket != INVALID_SOCKET) {

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
		m_hThread = INVALID_HANDLE_VALUE;
	}

	CLdServerSocket::CLdServerSocket()
		:m_hThread(INVALID_HANDLE_VALUE)
	{
	}

	CLdServerSocket::~CLdServerSocket()
	{
		Close();

		if (m_hThread != INVALID_HANDLE_VALUE)
			WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = INVALID_HANDLE_VALUE;
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

	BOOL CLdServerSocket::StartSelectThread()
	{
		CThread* thread = new CThread(this);
		m_hThread = thread->Start(0);
		return m_hThread != INVALID_HANDLE_VALUE;
	}

	void CLdServerSocket::DoAccept()
	{
		sockaddr_in addr_in;
		int nClientLength = sizeof(addr_in);

		SOCKET Socket = accept(m_Socket, (sockaddr*)&addr_in, &nClientLength);

		if (INVALID_SOCKET == Socket) {
			return;
		}

		CLdClientSocket* pClient = new CLdClientSocket(Socket);
		pClient->m_addr = addr_in.sin_addr;
		pClient->m_port = addr_in.sin_port;

		if (m_Listner && pClient)
			static_cast<IServerListener*>(m_Listner)->OnAccept(this, pClient);

		u_long nNoBlock = 1;
		ioctlsocket(Socket, FIONBIO, &nNoBlock);
		pClient->StartSelectThread();
	}

	void CLdServerSocket::ThreadBody(CThread* Sender, UINT_PTR Param)
	{
		fd_set ReadSet, WriteSet, ExceptSet;

		while (m_Socket != INVALID_SOCKET) {

			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);        //不处理send
			FD_ZERO(&ExceptSet);

			FD_SET(m_Socket, &ReadSet);
			FD_SET(m_Socket, &ExceptSet);

			if (select(0, &ReadSet, NULL, &ExceptSet, 0) > 0) {
				if (FD_ISSET(m_Socket, &ReadSet)) {
					DoAccept();
				}

				if (FD_ISSET(m_Socket, &ExceptSet)) {
					DoExcept();
				}
			}
			else
				break;
		}
	}

	void CLdServerSocket::OnThreadInit(CThread* Sender, UINT_PTR Param)
	{
	}

	void CLdServerSocket::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
	{
		m_hThread = INVALID_HANDLE_VALUE;
	}

}

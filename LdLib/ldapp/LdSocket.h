//
#pragma once

#include <winsock.h>
#include "../classes/Thread.h"
#include "../classes/LdArray.h"

#define SOCKET_PORT 0x5389

namespace LeadowLib
{
#pragma pack(push, 1)
	typedef struct LDSOCKET_DATA
	{
		WORD nSize;
		BYTE data[1];
	}*PLDSOCKET_DATA;
#pragma pack(pop)

	class CLdSocket;

	class ISocketListener //�����ӿڣ�����Socket�¼�
	{
	public:
		virtual void OnConnected(CLdSocket*) = 0;
		virtual void OnRecv(CLdSocket*, PBYTE pData, WORD nLength) = 0;
		virtual void OnClosed(CLdSocket*) = 0;
		virtual void OnAccept(CLdSocket*) = 0;
		virtual void OnError(CLdSocket*, int) = 0;
	};

	class CLdSocket
	{
		friend class CLdServerSocket;
	public:
		CLdSocket();
		virtual ~CLdSocket();

		bool IsClosed() const;
		UINT_PTR GetTag() const;
		VOID SetTag(UINT_PTR value);
		void SetListener(ISocketListener* listener);                          //���ü���
		ISocketListener* GetListener();

		void Close();
	protected:
		SOCKET m_Socket;
		IN_ADDR m_addr;
		UINT m_port;
		ISocketListener* m_Listner;

		bool m_Closed;
		UINT_PTR m_tag;   //ʹ�����Զ�������
		void DoExcept();
		void DoClose();
	};

	class CLdClientSocket :
		public CLdSocket,
		public IThreadRunable
	{
		friend class CLdServerSocket;
	public:
		CLdClientSocket(void);
		~CLdClientSocket(void);

		int Connect(LPCSTR szIp, int port = SOCKET_PORT);        //���ӷ����ַ���ͻ��ˣ�
		int Send(PVOID buffer, WORD nSize);                       //��������
		int Recv();
		PLDSOCKET_DATA GetRecvData() const;
		int GetRecvSize() const;
	private:
		CLdClientSocket(SOCKET s);

		PBYTE m_Buffer;
		int m_RecvSize;

		BOOL StartSelectThread();
		void DoRead();
//		void DoClientRead(CLdSocket* pClient);
//		void DoClientExcept(CLdSocket* pClient);
//		void DoClientClosed(CLdSocket* pClient);

		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

	class CLdServerSocket:
		public CLdSocket,
		public IThreadRunable
	{
	public:
		CLdServerSocket();
		~CLdServerSocket();
		BOOL Listen(int port = SOCKET_PORT);                              //ʹ��TCPЭ������˿ڣ�����ˣ�
		int GetClientCount();
		CLdClientSocket* GetClient(int idx);
	private:
		CLdArray<CLdClientSocket*> m_ClientSockets;
		BOOL StartSelectThread();
		void DoAccept();
		void RemoveClient(CLdClientSocket* pClient);
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
		CLdClientSocket* AddClient(SOCKET s);
	};
};
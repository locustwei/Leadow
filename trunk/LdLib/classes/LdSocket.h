
#pragma once

/*
�׽��֣����ڽ��̼�ͨ�š�
*/

#include <winsock.h>
#include "Thread.h"

#pragma warning(disable: 4200)

#define SOCKET_PORT 0x5389

namespace LeadowLib
{
#pragma pack(push, 1)
	typedef struct LDSOCKET_DATA
	{
		WORD nSize;
		BYTE data[0];
	}*PLDSOCKET_DATA;
#pragma pack(pop)

	class CLdSocket;

	interface ISocketListener //�����ӿڣ�����Socket�¼�
	{
		virtual void OnClosed(CLdSocket*) = 0;
		virtual void OnError(CLdSocket*, int) = 0;
		virtual void SetSocket(CLdSocket*) = 0;
	};

	class CLdSocket
	{
		//friend class CLdServerSocket;
	public:
		CLdSocket();
		virtual ~CLdSocket();

		bool IsClosed() const;
		UINT_PTR GetTag() const;
		VOID SetTag(UINT_PTR value);
		void SetListener(ISocketListener* listener);                          //���ü���
		ISocketListener* GetListener();
		void Close();
		SOCKET GetHandle();
	protected:
		SOCKET m_Socket;
		IN_ADDR m_addr;
		UINT m_port;
		ISocketListener* m_Listner;

		bool m_Closed;
		UINT_PTR m_tag;   //ʹ�����Զ�������
		void DoExcept();
		void DoClose();
	private:
		static int InitSocketDll();
		static int initsocket;   //��Ϊ�˳�ʼ��Socket ��̬��.
	};

	class CLdClientSocket :
		public CLdSocket,
		public IThreadRunable
	{
		//friend class CLdServerSocket;
	public:
		CLdClientSocket(void);
		~CLdClientSocket(void);

		int Connect(LPCSTR szIp = "127.0.0.1", int port = SOCKET_PORT);        //���ӷ����ַ���ͻ��ˣ�
		int Send(PVOID buffer, WORD nSize);                       //��������
		int Recv();
		PLDSOCKET_DATA GetRecvData() const;
		int GetRecvSize() const;
		BOOL StartSelectThread();
	private:
		HANDLE m_hThread;
		PBYTE m_Buffer;
		int m_RecvSize;
		
		void DoRead();
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

	interface IClientListener: public ISocketListener //�����ӿڣ�����Socket�¼�
	{
		virtual void OnConnected(CLdClientSocket*) = 0;
		virtual void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) = 0;
	};

	class CLdServerSocket:
		public CLdSocket,
		public IThreadRunable
	{
	public:
		CLdServerSocket();
		~CLdServerSocket();
		BOOL Listen(int port = SOCKET_PORT);                              //ʹ��TCPЭ������˿ڣ�����ˣ�
	protected:
		void DoAccept();
	private:
		HANDLE m_hThread;
		BOOL StartSelectThread();
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

	interface IServerListener: public ISocketListener //�����ӿڣ�����Socket�¼�
	{
		virtual void OnAccept(CLdServerSocket*, SOCKET) = 0;
	};
};
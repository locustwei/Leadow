
#pragma once

/*
套接字，用于进程间通信。
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

	interface ISocketListener //监听接口，处理Socket事件
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
		void SetListener(ISocketListener* listener);                          //设置监听
		ISocketListener* GetListener();
		void Close();
		SOCKET GetHandle();
	protected:
		SOCKET m_Socket;
		IN_ADDR m_addr;
		UINT m_port;
		ISocketListener* m_Listner;

		bool m_Closed;
		UINT_PTR m_tag;   //使用者自定义数据
		void DoExcept();
		void DoClose();
	private:
		static int InitSocketDll();
		static int initsocket;   //是为了初始化Socket 动态库.
	};

	class CLdClientSocket :
		public CLdSocket,
		public IThreadRunable
	{
		//friend class CLdServerSocket;
	public:
		CLdClientSocket(void);
		~CLdClientSocket(void);

		int Connect(LPCSTR szIp = "127.0.0.1", int port = SOCKET_PORT);        //连接服务地址（客户端）
		int Send(PVOID buffer, WORD nSize);                       //发送数据
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

	interface IClientListener: public ISocketListener //监听接口，处理Socket事件
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
		BOOL Listen(int port = SOCKET_PORT);                              //使用TCP协议监听端口（服务端）
	protected:
		void DoAccept();
	private:
		HANDLE m_hThread;
		BOOL StartSelectThread();
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

	interface IServerListener: public ISocketListener //监听接口，处理Socket事件
	{
		virtual void OnAccept(CLdServerSocket*, SOCKET) = 0;
	};
};
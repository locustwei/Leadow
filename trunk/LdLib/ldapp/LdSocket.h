//
#pragma once

#include <winsock.h>
#include "../classes/Thread.h"
#include "../classes/LdArray.h"

#define SOCKET_PORT 0x5389

namespace LeadowLib
{
	enum SOCKET_STATUS
	{
		SS_NONE,
		SS_CONNECTED,
		SS_BINDED,
		SS_LISTENING
	};

	class CSocketBase
	{
		friend class CLdSocket;
	public:
		CSocketBase();
		CSocketBase(SOCKET s);
		~CSocketBase();
		SOCKET GetSocket() const
		{ return m_Socket; };
		PVOID GetRecvData() const
		{ return lpRecvedBuffer; };
		int GetRecvSize() const
		{ return nRecvSize; };
		bool IsClosed() const
		{ return bClosed; };
		SOCKET_STATUS GetStatus() const
		{ return m_Status; };
		UINT_PTR GetTag() const
		{ return tag; };
		VOID SetTag(UINT_PTR value) { tag = value; };

		int Connect(LPCSTR szIp, int port = SOCKET_PORT);            //连接服务地址（客户端）
		int Send(PVOID buffer, int nSize);             //发送数据
		int Recv();
		void Close();
	protected:
		SOCKET m_Socket;
		IN_ADDR m_addr;
		UINT m_port;

		PUCHAR lpRecvedBuffer;
		int nRecvSize;
		bool bClosed;
		//_LD_CLIENT_SOCKET* pNext;
		UINT_PTR tag;   //使用者自定义数据
		SOCKET_STATUS m_Status;

	};// LD_CLIENT_SOCKET, *CClientSockt*;

	class ISocketListener //监听接口，处理Socket事件
	{
	public:
		virtual void OnConnected(CSocketBase*) = 0;
		virtual void OnRecv(CSocketBase*) = 0;
		virtual void OnClosed(CSocketBase*) = 0;
		virtual void OnAccept(CSocketBase*) = 0;
		virtual void OnError(CSocketBase*, int) = 0;
	};


	class CLdSocket :
		public CSocketBase,
		public IThreadRunable
	{
	public:
		CLdSocket(void);
		~CLdSocket(void);

//		BOOL ConnectTo(LPCSTR szIp, int port);                            //连接服务地址（客户端）
		BOOL Listen(int port = SOCKET_PORT);                                             //使用TCP协议监听端口（服务端）
		BOOL Bind(int port);                                              //使用UDP协议监听端口（服务端）
		CSocketBase* GetClient();                                        //Server 连接的客户端列表。
		void SetListener(ISocketListener* listener);                          //设置监听
		ISocketListener* GetListener();
	protected:
		//HANDLE m_hSelectThread;
		ISocketListener* m_Listner;
		CLdArray<CSocketBase*> m_ClientSockets;
		
		BOOL StartSelectThread();
		//CSocketBase* m_ClientHead;
		CSocketBase* AddClient(SOCKET s);
		void DoRead();
		void DoClientRead(CSocketBase* pClient);
		void DoClientExcept(CSocketBase* pClient);
		void DoAccept();
		//int RecvData(CSocketBase* pClient);
		//int GetSocketError(SOCKET Socket);
		void DoClientClosed(CSocketBase* pClient);
		void RemoveClient(CSocketBase* pClient);

		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};
};
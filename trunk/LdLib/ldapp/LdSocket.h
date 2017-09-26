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

	class CSocketBase;

	class ISocketListener //�����ӿڣ�����Socket�¼�
	{
	public:
		virtual void OnConnected(CSocketBase*) = 0;
		virtual void OnRecv(CSocketBase*, PBYTE pData, WORD nLength) = 0;
		virtual void OnClosed(CSocketBase*) = 0;
		virtual void OnAccept(CSocketBase*) = 0;
		virtual void OnError(CSocketBase*, int) = 0;
	};

	class CSocketBase
	{
	public:
		CSocketBase();
		CSocketBase(SOCKET s);
		virtual ~CSocketBase();

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

		bool bClosed;
		UINT_PTR tag;   //ʹ�����Զ�������

	};

	class CLdSocket :
		public CSocketBase,
		public IThreadRunable
	{
	public:
		CLdSocket(void);
		~CLdSocket(void);

		int Connect(LPCSTR szIp, int port = SOCKET_PORT);        //���ӷ����ַ���ͻ��ˣ�
		int Send(PVOID buffer, WORD nSize);                       //��������
		int Recv();
		PLDSOCKET_DATA GetRecvData() const;
		int GetRecvSize() const;
	private:
		PBYTE m_Buffer;
		int m_RecvSize;

		BOOL StartSelectThread();
		void DoRead();
//		void DoClientRead(CSocketBase* pClient);
//		void DoClientExcept(CSocketBase* pClient);
//		void DoClientClosed(CSocketBase* pClient);

		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

	class CLdServerSocket:
		public CSocketBase,
		public IThreadRunable
	{
	public:
		CLdServerSocket();
		~CLdServerSocket();
		BOOL Listen(int port = SOCKET_PORT);                              //ʹ��TCPЭ������˿ڣ�����ˣ�
		int GetClientCount();
		CSocketBase* GetClient(int idx);
	private:
		CLdArray<CSocketBase*> m_ClientSockets;
		BOOL StartSelectThread();
		void DoAccept();
		void RemoveClient(CSocketBase* pClient);
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
		CSocketBase* AddClient(SOCKET s);
	};
};
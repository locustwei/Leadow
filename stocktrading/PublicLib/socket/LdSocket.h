//
#pragma once

#include <winsock.h>

typedef enum SOCKET_STATUS
{
	SS_NONE,
	SS_CONNECTED,
	SS_BINDED,
	SS_LISTENING
};

typedef struct _LD_CLIENT_SOCKET
{
	SOCKET m_Socket;
	char* lpRecvedBuffer;
	int nRecvSize;
	bool bClosed;
	_LD_CLIENT_SOCKET* pNext;
	DWORD tag;   //ʹ�����Զ�������

}LD_CLIENT_SOCKET, *PLD_CLIENT_SOCKET;

struct ISocketListener //�����ӿڣ�����Socket�¼�
{
	virtual void OnConnected(PLD_CLIENT_SOCKET) = 0;
	virtual void OnRecv(PLD_CLIENT_SOCKET) = 0;
	virtual void OnClosed(PLD_CLIENT_SOCKET) = 0;
	virtual void OnAccept(PLD_CLIENT_SOCKET) = 0;
	virtual void OnError(PLD_CLIENT_SOCKET, int) = 0;
};


class CLdSocket: public LD_CLIENT_SOCKET
{
	friend DWORD WINAPI SocketSelectThreadProc(_In_ LPVOID lpParameter);
public:
	CLdSocket(void);
	~CLdSocket(void);
	
	BOOL ConnectTo(LPCSTR szIp, int port);                            //���ӷ����ַ���ͻ��ˣ�
	BOOL Listen(int port);                                             //ʹ��TCPЭ������˿ڣ�����ˣ�
	BOOL Bind(int port);                                              //ʹ��UDPЭ������˿ڣ�����ˣ�
	int Send(char* buffer, int nSize, PLD_CLIENT_SOCKET pClient = NULL);  //��������
	PLD_CLIENT_SOCKET GetClientHead();                                  //Server ���ӵĿͻ����б�
	void SetListener(ISocketListener* listener);                          //���ü���
	void Close();
	SOCKET_STATUS GetStatus();
	ISocketListener* GetListener();
protected:
	HANDLE m_hSelectThread;
	ISocketListener* m_Listner;
	SOCKET_STATUS m_Status;

	BOOL StartSelectThread();
	PLD_CLIENT_SOCKET m_ClientHead;
	PLD_CLIENT_SOCKET AddClient(SOCKET s);
	void DoRead();
	void DoClientRead(PLD_CLIENT_SOCKET pClient);
	void DoClientExcept(PLD_CLIENT_SOCKET pClient);
	void DoAccept();
	int RecvData(PLD_CLIENT_SOCKET pClient);
	int GetSocketError(SOCKET Socket);
	void DoClientClosed(PLD_CLIENT_SOCKET pClient);
	void RemoveClient(PLD_CLIENT_SOCKET pClient);
};


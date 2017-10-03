#pragma once

class CLdProcessCommunication
	:public CLdClientSocket
	,public IClientListener
{
public:
	CLdProcessCommunication();
	~CLdProcessCommunication();
	BOOL SendData(DWORD id, PVOID pData, WORD Length);
protected:
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnConnected(CLdClientSocket*) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
	void SetSocket(CLdSocket*) override;
	//CLdClientSocket* m_Socket;
};


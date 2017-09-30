#pragma once
class CLdProcessCommunication
	:public IClientListener
{
public:
	CLdProcessCommunication();
	~CLdProcessCommunication();
protected:
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnConnected(CLdClientSocket*) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
	void SetSocket(CLdSocket*) override;
	CLdClientSocket* m_Socket;
};


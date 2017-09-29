#pragma once

class CMainCommunication
	:public CLdServerSocket
	,public IServerListener
	,public IClientListener
{
public:
	CMainCommunication();
	~CMainCommunication();


private:
	CLdArray<CLdClientSocket*> m_Clients;
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnAccept(CLdServerSocket*, CLdClientSocket*) override;
	void OnConnected(CLdClientSocket*) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;
};

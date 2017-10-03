#pragma once

class CMainCommunication
	:public CLdServerSocket
	,public IServerListener
{
public:
	CMainCommunication();
	~CMainCommunication();
private:
	CLdArray<CLdClientSocket*> m_Clients;
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnAccept(CLdServerSocket*, SOCKET) override;
	void SetSocket(CLdSocket*) override;
};

#pragma once

class CServerListenerImpl
	:public IServerListener
{
public:
	void OnClosed(CLdSocket*) override;
	void OnError(CLdSocket*, int) override;
	void OnAccept(CLdServerSocket*, CLdClientSocket*) override;
};

class CMainCommunication
	:public CLdServerSocket
{
public:
	CMainCommunication();
	~CMainCommunication();
private:
	CServerListenerImpl m_Listener;
};


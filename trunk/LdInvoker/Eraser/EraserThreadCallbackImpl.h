#pragma once
#include "EraserCComm.h"

class CEraserThreadCallbackImpl:
	public IEraserThreadCallback
	,public CEraserCComm
{
public:
	CEraserThreadCallbackImpl();
	~CEraserThreadCallbackImpl();
protected:
	bool EraserThreadCallback(TCHAR* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

//	void SetSocket(CLdSocket*) override;
	void OnClosed(CLdSocket*) override;
//	void OnError(CLdSocket*, int) override;
//	void OnConnected(CLdClientSocket*) override;
	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;

private:
	bool m_Abort;
};


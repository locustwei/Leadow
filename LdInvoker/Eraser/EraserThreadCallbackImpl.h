#pragma once
#include "EraserCComm.h"
#include "../../LdFileEraser/ErasureLibrary.h"

class CEraserThreadCallbackImpl:
	public IEraserThreadCallback
{
public:
	CEraserThreadCallbackImpl();
	~CEraserThreadCallbackImpl();
protected:
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

//	void SetSocket(CLdSocket*) override;
//	void OnClosed(CLdSocket*) override;
//	void OnError(CLdSocket*, int) override;
//	void OnConnected(CLdClientSocket*) override;
//	void OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength) override;

private:
	CEraserCComm m_Socket;
	bool m_Abort;
};


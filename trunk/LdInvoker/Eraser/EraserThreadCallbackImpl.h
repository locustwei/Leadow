#pragma once
#include "../../LdFileEraser/ErasureLibrary.h"

class CEraserThreadCallbackImpl:
	public IEraserThreadCallback
	, ISocketListener
{
public:
	CEraserThreadCallbackImpl();
	~CEraserThreadCallbackImpl();
protected:
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;

	void OnConnected(CSocketBase*) override;
	void OnRecv(CSocketBase*, PBYTE pData, WORD nLength) override;
	void OnClosed(CSocketBase*) override;
	void OnAccept(CSocketBase*) override;
	void OnError(CSocketBase*, int) override;

private:
	CLdClientSocket m_socket;
	bool m_Abort;
};


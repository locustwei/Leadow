#include "stdafx.h"
#include "EraserThreadCallbackImpl.h"

CEraserThreadCallbackImpl::CEraserThreadCallbackImpl()
	:m_Abort(false)
{
}

CEraserThreadCallbackImpl::~CEraserThreadCallbackImpl()
{
}

bool CEraserThreadCallbackImpl::EraserThreadCallback(TCHAR* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	//if (m_Socket.IsClosed())
		//return true;
	SendEraseStatus(pFile, op, dwValue);

	if (op == eto_finished)
		PostQuitMessage(0);

	return !m_Abort;
}

void CEraserThreadCallbackImpl::OnClosed(CLdSocket*)
{
	m_Abort = true;
}

void CEraserThreadCallbackImpl::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{

}

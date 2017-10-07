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
	if (pFile)
		SendEraseStatus(pFile, op, dwValue);
	else
		SendEraseStatus(nullptr, op, dwValue);

	return !m_Abort;
}

void CEraserThreadCallbackImpl::OnClosed(CLdSocket*)
{
	m_Abort = true;
}

void CEraserThreadCallbackImpl::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{

}

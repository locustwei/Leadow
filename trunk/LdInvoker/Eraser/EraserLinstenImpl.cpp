#include "stdafx.h"
#include "EraserLinstenImpl.h"
#include "../LdApp/LdApp.h"

CEraserListenImpl::CEraserListenImpl()
	:m_Abort(false)
{
}

CEraserListenImpl::~CEraserListenImpl()
{
}

bool CEraserListenImpl::AnalyResult(TCHAR* FileName, PVOID pData)
{
	return true;
}

bool CEraserListenImpl::EraserReprotStatus(TCHAR* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	//if (m_Socket.IsClosed())
		//return true;
	SendEraseStatus(pFile, op, dwValue);

	if (op == eto_finished)
		PostThreadMessage(ThisApp->GetMainThreadId(), WM_QUIT, 0, 0);

	return !m_Abort;
}

void CEraserListenImpl::OnClosed(CLdSocket*)
{
	m_Abort = true;
}

void CEraserListenImpl::OnRecv(CLdClientSocket*, PBYTE pData, WORD nLength)
{
	PCOMMUINCATION_DATA data = (PCOMMUINCATION_DATA)pData;
	PCOMM_ERASE_DATA pEraseData = (PCOMM_ERASE_DATA)data->Data;

	switch(pEraseData->op)
	{
	case eto_abort: 
		m_Abort = true;
		break;
	}
}

void CEraserListenImpl::SetSocket(CLdSocket*)
{
}

void CEraserListenImpl::SetContext(PVOID)
{
}

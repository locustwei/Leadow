#include "stdafx.h"
#include  "EraserThreadCallbackImpl.h"

CEraserThreadCallbackImpl::CEraserThreadCallbackImpl()
	:m_socket()
	,m_Abort(false)
{
	m_socket.SetListener(this);
	m_socket.Connect("127.0.0.1");
}

CEraserThreadCallbackImpl::~CEraserThreadCallbackImpl()
{
	m_socket.Close();
}

bool CEraserThreadCallbackImpl::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	if (m_socket.IsClosed())
		return true;
	switch (op)
	{
	case eto_start:  //�ܽ��ȿ�ʼ
		m_socket.Send(&op, sizeof(op));
		break;
	case eto_begin:
		break;
	case eto_completed: //�����ļ��������
						//���ò���״̬

		break;
	case eto_progress: //�����ļ�����

		break;
	case eto_finished:

		break;
	default:
		break;
	}
	return !m_Abort;
}

void CEraserThreadCallbackImpl::OnConnected(CSocketBase *)
{

}

void CEraserThreadCallbackImpl::OnRecv(CSocketBase*, PBYTE pData, WORD nLength)
{
	PVOID buffer = m_socket.GetRecvData();

}

void CEraserThreadCallbackImpl::OnClosed(CSocketBase *)
{
}

void CEraserThreadCallbackImpl::OnAccept(CSocketBase *)
{
}

void CEraserThreadCallbackImpl::OnError(CSocketBase *, int)
{
}

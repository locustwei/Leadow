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
	case eto_start:  //总进度开始
		m_socket.Send(&op, sizeof(op));
		break;
	case eto_begin:
		break;
	case eto_completed: //单个文件擦除完成
						//设置擦除状态

		break;
	case eto_progress: //单个文件进度

		break;
	case eto_finished:

		break;
	default:
		break;
	}
	return !m_Abort;
}

void CEraserThreadCallbackImpl::OnRecv(CSocketBase*)
{
	PVOID buffer = m_socket.GetRecvData();

}

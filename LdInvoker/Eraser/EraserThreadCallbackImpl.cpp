#include "stdafx.h"
#include "EraserThreadCallbackImpl.h"

CEraserThreadCallbackImpl::CEraserThreadCallbackImpl()
	:m_Abort(false)
	, m_Socket()
{
	m_Socket.Connect();
}

CEraserThreadCallbackImpl::~CEraserThreadCallbackImpl()
{
}

bool CEraserThreadCallbackImpl::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	//if (m_Socket.IsClosed())
		//return true;
	m_Socket.SendData(op, &dwValue, sizeof(DWORD));

	switch (op)
	{
	case eto_start:  //总进度开始
		//m_Socket.Send(&op, sizeof(op));
		break;
	case eto_begin:
		break;
	case eto_completed: //单个文件擦除完成
						//设置擦除状态

		break;
	case eto_progress: //单个文件进度

		break;
	case eto_finished:
		delete this;
		PostThreadMessage(CLdApp::ThisApp->GetMainThreadId(), WM_QUIT, 0, 0);
		break;
	default:
		break;
	}
	return !m_Abort;
}



#include "stdafx.h"
#include "Thread.h"


CThread::CThread(void)
{
	ThreadInit();
}

CThread::CThread(IRunable* pRumer, WPARAM Param)
{
	ThreadInit();
	m_Param = Param;
	m_Runer = pRumer;
}


CThread::~CThread(void)
{
	Terminate(0);
}

int CThread::GetPriority()
{
	return ::GetThreadPriority((HANDLE)m_hThread);
}

BOOL CThread::SetPriority(int nPriority)
{
	return (::SetThreadPriority((HANDLE)m_hThread, nPriority) == TRUE); 
}

DWORD CThread::Suspend()
{
	return ::SuspendThread(m_hThread);
}

DWORD CThread::Resume()
{
	 return ::ResumeThread(m_hThread); 
}

HANDLE CThread::GetHandle() const
{
	return m_hThread;
}

DWORD CThread::GetId() const
{
	 return m_ThreadId; 
}

int CThread::Start()
{
	if (m_hThread != INVALID_HANDLE_VALUE)		
		return -1;

	if(m_Runer)
		m_Runer->OnThreadInit(m_Param);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProcedure, this, 0, (unsigned int*)&m_ThreadId);

	if (!m_hThread)	
		return -3;

	return 0;
}

VOID CThread::ResetHandle()
{
	if (m_hThread != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
	}

	m_ThreadId = 0;

}

unsigned __stdcall CThread::ThreadProcedure(LPVOID pParam)
{
	CThread* pThis = (CThread*)pParam;

	if (!pThis) return 1;
	int uRet = pThis->ThreadRun();

	pThis->ResetHandle();

	pThis->DoTerminated();

	if(pThis->GetFreeOnTerminate())
		delete pThis;

	return (unsigned)uRet ;

}

DWORD CThread::Terminate(DWORD uExitCode, DWORD dwWaitTime /*= 0*/)
{
	if (m_hThread == INVALID_HANDLE_VALUE) return uExitCode;
	
	m_Terminated = TRUE;
	
	if(dwWaitTime !=0)
		::WaitForSingleObject(m_hThread, dwWaitTime);

	TerminateThread((HANDLE)m_hThread, uExitCode);

	ResetHandle();

	return uExitCode;

}

BOOL CThread::IsAlive()
{
	return (m_hThread != NULL);
}


bool CThread::GetFreeOnTerminate()
{
	return m_FreeOnTerminate;
}

VOID CThread::SetFreeOnTerminate(bool value)
{
	m_FreeOnTerminate = value;
}

int CThread::ThreadRun()
{
	if(m_Runer)
		m_Runer->ThreadRun(m_Param);

	return 0;
}

void CThread::ThreadInit()
{
	m_hThread = INVALID_HANDLE_VALUE;
	m_ThreadId = 0;
	m_Terminated = FALSE;
	m_FreeOnTerminate = true;
	m_Runer = NULL;
	m_Param = 0;
}

void CThread::DoTerminated()
{
	if(m_Runer)
		m_Runer->OnThreadTerminated(m_Param);
}
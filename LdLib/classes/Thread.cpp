#include "stdafx.h"
#include "Thread.h"

namespace LeadowLib {
	CThread::CThread(void)
	{
		ThreadInit();
	}

	CThread::CThread(IThreadRunable* pRumer)
	{
		ThreadInit();
		m_Runer = pRumer;
	}

	CThread::~CThread(void)
	{
		if (m_hSleep)
			CloseHandle(m_hSleep);
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

	HANDLE CThread::Start(UINT_PTR Param)
	{
		if (m_hThread != INVALID_HANDLE_VALUE)
			return INVALID_HANDLE_VALUE;

		m_Param = Param;
		m_Terminated = false;

		if (m_Runer)
			m_Runer->OnThreadInit(this, m_Param);

		HANDLE hThread = CreateThread(nullptr, 0, &ThreadProcedure, this, CREATE_SUSPENDED, &m_ThreadId);
		m_hThread = hThread;  //这么啰嗦防止线程过早结束this无效
		if (hThread != 0)
			ResumeThread(hThread);
		return hThread;
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

	DWORD WINAPI CThread::ThreadProcedure(LPVOID pParam)
	{
		CThread* pThis = (CThread*)pParam;

		if (!pThis) return 1;

		//pThis->DoStart();

		int uRet = pThis->ThreadRun();

		pThis->DoTerminated();
		pThis->ResetHandle();

		if (pThis->GetFreeOnTerminate())
			delete pThis;

		return (unsigned)uRet;

	}

	DWORD CThread::Terminate(DWORD dwWaitTime /*= 0*/, DWORD uExitCode)
	{
		if (m_hThread == INVALID_HANDLE_VALUE) return uExitCode;

		SetTerminatee(TRUE);

		if (dwWaitTime != 0)
			::WaitForSingleObject(m_hThread, dwWaitTime);

		TerminateThread(m_hThread, uExitCode);

		ResetHandle();

		return uExitCode;

	}

	DWORD CThread::Sleep(DWORD dwMilliseconds)
	{
		if (!m_hSleep)
		{
			m_hSleep = CreateEvent(nullptr, TRUE, false, nullptr);
			if (m_hSleep == nullptr)
				return GetLastError();
		}
		else
			ResetEvent(m_hSleep);
		return WaitForSingleObject(m_hSleep, dwMilliseconds);
	}

	VOID CThread::Wakeup(HANDLE hEvent) const
	{
		if (m_hSleep)
		{
			SetEvent(m_hSleep);
		}
	}

	BOOL CThread::IsAlive() const
	{
		return (m_hThread != INVALID_HANDLE_VALUE);
	}


	bool CThread::GetFreeOnTerminate() const
	{
		return m_FreeOnTerminate;
	}

	VOID CThread::SetFreeOnTerminate(bool value)
	{
		m_FreeOnTerminate = value;
	}

	bool CThread::GetTerminated() const
	{
		return m_Terminated;
	}

	VOID CThread::SetTerminatee(bool value)
	{
		Wakeup(m_hSleep);
		m_Terminated = value;
	}

	int CThread::ThreadRun()
	{
		if (m_Runer)
			m_Runer->ThreadRun(this, m_Param);

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
		m_hSleep = nullptr;
	}

	void CThread::DoTerminated()
	{
		if (m_Runer)
			m_Runer->OnThreadTerminated(this, m_Param);
	}
}
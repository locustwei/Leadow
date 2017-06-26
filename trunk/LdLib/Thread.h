#pragma once

#include "stdafx.h"
#include <process.h>

class IRunable{
public:
	virtual VOID ThreadRun(WPARAM Param) = 0;
	virtual VOID OnThreadInit(WPARAM Param) = 0;
	virtual VOID OnThreadTerminated(WPARAM Param) = 0;
};

class CThread
{
private:
	CThread(void);
	CThread(IRunable* pRumer, WPARAM Param);
	~CThread(void);
public:
	static CThread* NewThread(IRunable* pRumer, WPARAM Param = 0);
	int	GetPriority();
	BOOL SetPriority(int nPriority);
	DWORD Suspend();
	DWORD Resume();
	HANDLE GetHandle() const;
	DWORD GetId() const;
	BOOL  IsAlive();
	bool GetFreeOnTerminate();
	VOID SetFreeOnTerminate(bool value);

	virtual int Start();
	virtual DWORD Terminate(DWORD uExitCode, DWORD dwWaitTime = 0);
protected:
	virtual int	ThreadRun();
	virtual void ThreadInit();
	BOOL m_Terminated;
private:
	volatile HANDLE	m_hThread;		
	volatile DWORD	m_ThreadId;	
	IRunable* m_Runer;
	bool m_FreeOnTerminate;
	WPARAM m_Param;
	VOID ResetHandle();
	static unsigned __stdcall ThreadProcedure(LPVOID pParam);
	void DoTerminated();
};


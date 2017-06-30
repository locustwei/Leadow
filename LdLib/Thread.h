#pragma once

#include "stdafx.h"
#include <process.h>

class CThread;

struct IRunable{
	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) = 0;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) = 0;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) = 0;
};

class CThread
{
public:
	CThread(void);
	CThread(IRunable* pRumer, WPARAM Param);
	~CThread(void);

	int	GetPriority();
	BOOL SetPriority(int nPriority);
	DWORD Suspend();
	DWORD Resume();
	HANDLE GetHandle() const;
	DWORD GetId() const;
	BOOL  IsAlive() const;
	
	bool GetFreeOnTerminate() const;
	VOID SetFreeOnTerminate(bool value);
	bool GetTerminated() const;
	VOID SetTerminatee(bool value);

	virtual int Start();
	virtual DWORD Terminate(DWORD dwWaitTime = 0, DWORD uExitCode = 0);
	DWORD Sleep(DWORD dwMilliseconds);
	VOID Wakeup(HANDLE hEvent) const;
protected:
	virtual int	ThreadRun();
	virtual void ThreadInit();
	BOOL m_Terminated;
	HANDLE m_hSleep;
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


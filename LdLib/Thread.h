#pragma once

#include "stdafx.h"

class CThread;

class IThreadRunable{
public:
	virtual VOID ThreadRun(CThread* Sender, UINT_PTR Param) = 0;
	//线程还么有启动，在主线程内
	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) = 0;  
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) = 0;
};

class CThread
{
public:
	CThread(void);
	CThread(IThreadRunable* pRumer);
	virtual ~CThread(void);

	int	GetPriority();
	BOOL SetPriority(int nPriority);
	DWORD Suspend();
	DWORD Resume();
	HANDLE GetHandle() const;
	DWORD GetId() const;  //线程Id
	BOOL  IsAlive() const;
	
	bool GetFreeOnTerminate() const;
	VOID SetFreeOnTerminate(bool value);
	bool GetTerminated() const;
	VOID SetTerminatee(bool value);

	/*
	开始线程
	Param 回传到线程回掉函数
	*/
	virtual HANDLE Start(UINT_PTR Param);
	/*
	结束线程
	dwWaitTime 等待线程结束时间，
	uExitCode 线程结束返回值
	*/
	virtual DWORD Terminate(DWORD dwWaitTime = 0, DWORD uExitCode = 0);
	/*
	替换系统Sleep函数可以使用Wakeup结束Sleep
	dwMilliseconds 睡眠时间
	*/
	DWORD Sleep(DWORD dwMilliseconds);
	VOID Wakeup(HANDLE hEvent) const;
protected:
	virtual int	ThreadRun();
	virtual void ThreadInit();
	BOOL m_Terminated;
	HANDLE m_hSleep;
private:
	HANDLE m_hThread;		
	DWORD m_ThreadId;	
	IThreadRunable* m_Runer;
	bool m_FreeOnTerminate;
	WPARAM m_Param;
	VOID ResetHandle();
	static DWORD WINAPI ThreadProcedure(LPVOID pParam);
	void DoTerminated();
};


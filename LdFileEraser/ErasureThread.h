#pragma once
#include "Erasure.h"

//待擦除文件记录
typedef struct ERASURE_FILE_DATA
{
	TCHAR cFileName[MAX_PATH];
	bool IsDirectory;
	bool Ereased;   //已被擦除过标记
	CControlUI* ListRow; //对应在界面的UI，擦除后删除之（不要在擦除线程里找UI同步很难处理）
}*PERASURE_FILE_DATA;


class CErasureThread: public CThread, public IErasureCallback
{
public:
	CErasureThread(IThreadRunable* run);
	~CErasureThread();

	CProgressUI* ui;

	virtual BOOL ErasureStart(UINT nStepCount) override;
	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;

//	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
//	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
//	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};

#define MAX_THREAD_COUNT 6  //同时开启文件擦除线程数
//擦除线程数组，
class CEreaserThrads : public CLdArray<CThread*>
{
public:
	CEreaserThrads()
	{
		InitializeCriticalSection(&cs);
	}

	void StopThreads()
	{
		EnterCriticalSection(&cs);
		for (int i = 0; i < GetCount(); i++)
			Get(i)->SetTerminatee(true);
		LeaveCriticalSection(&cs);
	};

	CThread* AddThread(IThreadRunable* run, UINT_PTR Param);

	void RemoveThread(CThread* thread)
	{
		EnterCriticalSection(&cs);
		Remove(thread);
		LeaveCriticalSection(&cs);
	};
private:
	CRITICAL_SECTION cs;
};
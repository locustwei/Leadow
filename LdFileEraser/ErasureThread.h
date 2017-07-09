#pragma once
#include "Erasure.h"

//�������ļ���¼
typedef struct ERASURE_FILE_DATA
{
	TCHAR cFileName[MAX_PATH];
	bool IsDirectory;
	bool Ereased;   //�ѱ����������
	CControlUI* ListRow; //��Ӧ�ڽ����UI��������ɾ��֮����Ҫ�ڲ����߳�����UIͬ�����Ѵ���
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

#define MAX_THREAD_COUNT 6  //ͬʱ�����ļ������߳���
//�����߳����飬
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
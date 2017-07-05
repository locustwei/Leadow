#pragma once
#include "Erasure.h"

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


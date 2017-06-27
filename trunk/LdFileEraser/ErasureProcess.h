#pragma once
#include "Erasure.h"

class CErasureProcess: public IErasureCallback, public IRunable
{
public:
	CErasureProcess();
	~CErasureProcess();

	CProgressUI* ui;

	virtual BOOL ErasureStart(UINT nStepCount) override;
	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;

	virtual VOID ThreadRun(WPARAM Param) override;
	virtual VOID OnThreadInit(WPARAM Param) override;
	virtual VOID OnThreadTerminated(WPARAM Param) override;

};


#pragma once

#include "ErasureMethod.h"

class IErasureCallback
{
public:
	virtual BOOL ErasureStart(UINT nStepCount) = 0;
	virtual VOID ErasureCompleted(DWORD dwErroCode) = 0;
	virtual BOOL ErasureProgress(int nStep, UINT nMax, UINT nCurent) = 0;
};

class CUnuseSpaceErasure
{
public:
	CUnuseSpaceErasure();
	~CUnuseSpaceErasure();

	DWORD Execute(CLdString& Driver, CErasureMethod& method, IErasureCallback* callback);
private:
	CVolumeInfo m_volInfo;
	CLdString m_tmpDir;
	IErasureCallback* m_callback;
	CErasureMethod* m_method;

	DWORD EraseUnusedSpace();
};


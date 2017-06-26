#include "stdafx.h"
#include "ErasureProcess.h"



CErasureProcess::CErasureProcess()
{
}


CErasureProcess::~CErasureProcess()
{
}

BOOL CErasureProcess::ErasureStart(UINT nStepCount)
{
	return TRUE;
}

BOOL CErasureProcess::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	return TRUE;
}

BOOL CErasureProcess::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	return TRUE;
}

VOID CErasureProcess::ThreadRun(WPARAM Param)
{
}

VOID CErasureProcess::OnThreadInit(WPARAM Param)
{
}

VOID CErasureProcess::OnThreadTerminated(WPARAM Param)
{
}

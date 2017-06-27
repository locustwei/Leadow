#include "stdafx.h"
#include "ErasureProcess.h"



CErasureProcess::CErasureProcess()
{
	ui = NULL;
}


CErasureProcess::~CErasureProcess()
{
}

BOOL CErasureProcess::ErasureStart(UINT nStepCount)
{
	if (ui)
	{

	}
	return TRUE;
}

BOOL CErasureProcess::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (ui)
	{

	}
	return TRUE;
}

BOOL CErasureProcess::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (ui)
	{

	}
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

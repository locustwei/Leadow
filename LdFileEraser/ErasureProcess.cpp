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
		ui->SetVisible(true);
	}
	return TRUE;
}

BOOL CErasureProcess::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (ui)
	{
		ui->SetVisible(false);
	}
	return TRUE;
}

BOOL CErasureProcess::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)
		return FALSE;

	if (ui)
	{
		//ui->SetMaxValue(nMaxCount);
		ui->SetValue(nCurent * 100 / nMaxCount);
	}
	return TRUE;
}

VOID CErasureProcess::ThreadRun(CThread* Sender, WPARAM Param)
{
}

VOID CErasureProcess::OnThreadInit(CThread* Sender, WPARAM Param)
{
}

VOID CErasureProcess::OnThreadTerminated(CThread* Sender, WPARAM Param)
{
}

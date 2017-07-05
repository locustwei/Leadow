#include "stdafx.h"
#include "ErasureProcess.h"

CErasureThread::CErasureThread(IThreadRunable* run):CThread(run)
{
	ui = NULL;
}


CErasureThread::~CErasureThread()
{
}

BOOL CErasureThread::ErasureStart(UINT nStepCount)
{
	if (ui)
	{
		ui->SetVisible(true);
	}
	return !GetTerminated();
}

BOOL CErasureThread::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (ui)
	{
		ui->SetVisible(false);
	}
	return !GetTerminated();
}

BOOL CErasureThread::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)
		return FALSE;

	if (ui)
	{
		//ui->SetMaxValue(nMaxCount);
		ui->SetValue(nCurent * 100 / nMaxCount);
	}
	return !GetTerminated();
}

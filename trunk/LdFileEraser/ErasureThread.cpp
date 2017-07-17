#include "stdafx.h"
#include "ErasureThread.h"

CErasureThread::CErasureThread(PERASURE_FILE_DATA pEraseFile)
{
	m_Data = pEraseFile;
}


CErasureThread::~CErasureThread()
{
}

BOOL CErasureThread::ErasureStart(UINT nStepCount)
{
	if (m_Data && m_Data->pProgress)
	{
		m_Data->pProgress->SetVisible(true);
	}
	return m_Data->nStatus != efs_abort;
}

BOOL CErasureThread::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (m_Data && m_Data->pProgress)
	{
		m_Data->pProgress->SetVisible(false);
	}
	return m_Data->nStatus != efs_abort;
}

BOOL CErasureThread::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)
		return FALSE;

	if (m_Data && m_Data->pProgress)
	{
		m_Data->pProgress->SetValue(nCurent * 100 / nMaxCount);
	}
	return m_Data->nStatus != efs_abort;
}

VOID CErasureThread::ThreadRun(CThread * Sender, WPARAM Param)
{
	if (Sender->GetTerminated())
		return;
	if (!m_Data)
		return;

	CErasure erasure;
	switch(m_Data->nFileType)
	{
	case eft_file: 
	case eft_directory: 
		m_Data->nErrorCode = erasure.FileErasure(m_Data->cFileName, CErasureMethod::Pseudorandom(), this);
		break;
	case eft_volume: 
		m_Data->nErrorCode = erasure.UnuseSpaceErasure(m_Data->cFileName, CErasureMethod::Pseudorandom(), this);
		break;
	default: 
		break;
	}

	if (m_Data->nErrorCode == 0)
	{
		m_Data->nStatus = efs_erasured;
	}else
	{
		m_Data->nStatus = efs_error;
	}
}

VOID CErasureThread::OnThreadInit(CThread * Sender, WPARAM Param)
{
	return VOID();
}

VOID CErasureThread::OnThreadTerminated(CThread * Sender, WPARAM Param)
{
	return VOID();
}

CEreaserThrads::CEreaserThrads():
	m_Threads(),
	m_Files()
{
	InitializeCriticalSection(&cs);
}

void CEreaserThrads::StopThreads()
{
	EnterCriticalSection(&cs);
	for (int i = 0; i < m_Threads.GetCount(); i++)
		m_Threads.Get(i)->SetTerminatee(true);
	LeaveCriticalSection(&cs);
}

void CEreaserThrads::AddEreaureFile(PERASURE_FILE_DATA pFile)
{
}

DWORD CEreaserThrads::StartEreasure(UINT nMaxCount)
{
	CThread* thread = new CThread(this);
	thread->Start(nMaxCount);
}

void CEreaserThrads::ThreadRun(CThread * Sender, UINT_PTR Param)
{
	WaitForMultipleObjects()
	for (int i = 0; i < m_Files.GetCount(); i++)
	{//先擦除文件
		if (Sender->GetTerminated())
			break;

		PERASURE_FILE_DATA pinfo = m_Files[i];
		if (pinfo->nStatus == efs_erasured || pinfo->nFileType != eft_file)
			continue;

		while (m_Threads.GetCount() >= Param)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}
		CErasureThread* eraser = new CErasureThread(pinfo);
		CThread* thread = new CThread(eraser);
		thread->Start(0);
		m_Threads.Add(eraser);
	}

	if (Sender->GetTerminated())
		return;

	for (int i = 0; i < m_Files.GetCount(); i++)
	{//目录
		if (Sender->GetTerminated())
			break;

		PERASURE_FILE_DATA pinfo = m_Files[i];
		if (pinfo->nStatus == efs_erasured || pinfo->nFileType == eft_file)
			continue;

		while (m_Threads.GetCount() >= Param)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}
		CErasureThread* eraser = new CErasureThread(pinfo);
		CThread* thread = new CThread(eraser);

		m_Threads.Add(eraser);
	}
}

void CEreaserThrads::OnThreadInit(CThread * Sender, UINT_PTR Param)
{
}

void CEreaserThrads::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
{
}

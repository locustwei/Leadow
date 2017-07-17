#include "stdafx.h"
#include "ErasureThread.h"

CEreaserThrads::CEreaserThrads(IGernalCallback<PERASE_CALLBACK_PARAM>* callback):
	m_Files()
{
	m_callback = callback;
	m_Abort = false;
	m_hEvent = nullptr;
	m_nMaxThreadCount = MAX_THREAD_COUNT;
}

CEreaserThrads::~CEreaserThrads()
{
	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

void CEreaserThrads::StopThreads()
{
	m_Abort = true;
	SetEvent(m_hEvent);
}

void CEreaserThrads::AddEreaureFile(PERASURE_FILE_DATA pFile)
{
	m_Files.Add(pFile);
}

DWORD CEreaserThrads::StartEreasure(UINT nMaxCount)
{
	if (m_hEvent == NULL)
		m_hEvent = CreateEvent(nullptr, true, false, nullptr);

	m_nMaxThreadCount = nMaxCount;
	CThread* thread = new CThread(this);
	thread->Start(0);
	return 0;
}

void CEreaserThrads::ControlThreadRun()
{
	m_Files.Sort(this);
	HANDLE* threads = new HANDLE[m_nMaxThreadCount + 1];
	ZeroMemory(threads, sizeof(HANDLE)*(m_nMaxThreadCount + 1));
	threads[0] = m_hEvent;
	int k = 1;
	boolean bWait = false;
	for (int i = 0; i < m_Files.GetCount(); i++)
	{
		if (m_Abort)
			break;
		if(bWait)
		{
			DWORD obj = WaitForMultipleObjects(k, threads, FALSE, INFINITE);
			if (obj == WAIT_OBJECT_0)
				break;
			else
				k = obj - WAIT_OBJECT_0;
		}
		PERASURE_FILE_DATA pinfo = m_Files[i];
		if (pinfo->nStatus == efs_erasured)
			continue;

		CThread* thread = new CThread(this);
		thread->Start((UINT_PTR)pinfo);
		threads[k++] = thread->GetHandle();
		bWait = k >= m_nMaxThreadCount;
	}
	delete [] threads;

	ERASE_CALLBACK_PARAM Param;
	Param.op = eto_finished;
	m_callback->GernalCallback_Callback(&Param, 0);
}

void CEreaserThrads::ErasureThreadRun(PERASURE_FILE_DATA pData)
{
	if (m_Abort)
		return;

	CErasure erasure;
	ERASE_CALLBACK_PARAM Param;
	Param.pData = pData;
	Param.op = eto_start;
	Param.nPercent = 0;

	if (!m_callback->GernalCallback_Callback(&Param, 0))
		return;

	CErasureCallbackImpl impl(&Param);
	impl.threads = this;

	switch (pData->nFileType)
	{
	case eft_file:
	case eft_directory:
		pData->nErrorCode = erasure.FileErasure(pData->cFileName, pData->pMethod, &impl);
		break;
	case eft_volume:
		pData->nErrorCode = erasure.UnuseSpaceErasure(pData->cFileName, pData->pMethod, &impl);
		break;
	default:
		break;
	}

	if (pData->nErrorCode == 0)
	{
		pData->nStatus = efs_erasured;
	}
	else
	{
		pData->nStatus = efs_error;
	}
}

void CEreaserThrads::ThreadRun(CThread * Sender, UINT_PTR Param)
{
	if (Param == 0)
		ControlThreadRun();
	else
		ErasureThreadRun((PERASURE_FILE_DATA)Param);
}

void CEreaserThrads::OnThreadInit(CThread * Sender, UINT_PTR Param)
{
	
}

void CEreaserThrads::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
{
}

int CEreaserThrads::ArraySortCompare(ERASURE_FILE_DATA ** it1, ERASURE_FILE_DATA ** it2)
{
	return 0;
}

CEreaserThrads::CErasureCallbackImpl::CErasureCallbackImpl(PERASE_CALLBACK_PARAM pData)
{
	m_Data = pData;
	threads = nullptr;
}


CEreaserThrads::CErasureCallbackImpl::~CErasureCallbackImpl()
{
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureStart(UINT nStepCount)
{
	if (threads->m_Abort)
		return false;

	m_Data->op = eto_start;
	if (!threads->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (threads->m_Abort)
		return false;

	m_Data->op = eto_completed;
	if (!threads->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)
		return FALSE;

	if (threads->m_Abort)
		return false;

	m_Data->op = eto_progress;
	m_Data->nPercent = nCurent * 100 / nMaxCount;
	if (!threads->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;
	return true;
}
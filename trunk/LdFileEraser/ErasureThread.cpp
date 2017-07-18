#include "stdafx.h"
#include "ErasureThread.h"

CEreaserThrads::CEreaserThrads(IGernalCallback<PERASE_CALLBACK_PARAM>* callback):
	m_Files()
{
	m_callback = callback;
	m_Abort = false;
	m_hEvent = nullptr;
	m_nMaxThreadCount = MAX_THREAD_COUNT;
	m_ControlThread = nullptr;
}

CEreaserThrads::~CEreaserThrads()
{
	StopThreads();

	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
}

void CEreaserThrads::StopThreads()
{
	m_Abort = true;
	if(m_hEvent)
		SetEvent(m_hEvent);
}

void CEreaserThrads::AddEreaureFile(PERASURE_FILE_DATA pFile)
{
	m_Files.Add(pFile);
}

DWORD CEreaserThrads::StartEreasure(UINT nMaxCount)
{
	if (m_ControlThread)  //上一次擦除还没有结束
		return (DWORD)-1;

	if (m_hEvent == NULL)
		m_hEvent = CreateEvent(nullptr, true, false, nullptr);
	else
		ResetEvent(m_hEvent);
	
	m_Abort = false;

	m_nMaxThreadCount = nMaxCount;
	m_ControlThread = new CThread(this);
	m_ControlThread->Start(0);
	return 0;
}
//擦除控制线程
void CEreaserThrads::ControlThreadRun()
{
	ERASE_CALLBACK_PARAM Param;
	Param.op = eto_begin;
	m_callback->GernalCallback_Callback(&Param, 0);

	m_Files.Sort(this);  //按文件->子目录->父目录排序
	
	//保持当前擦除线程
	HANDLE* threads = new HANDLE[m_nMaxThreadCount + 1];             
	ZeroMemory(threads, sizeof(HANDLE)*(m_nMaxThreadCount + 1));
	threads[0] = m_hEvent;
	int k = 1;
	boolean bWait = false;
	for (int i = 0; i < m_Files.GetCount(); i++)
	{
		if (m_Abort)
			break;
		if(bWait)  //线程已满等待其中一个结束
		{
			DWORD obj = WaitForMultipleObjects(k, threads, FALSE, INFINITE);
			if (obj == WAIT_OBJECT_0)
			{   //终止擦除，等待其他线程都结束。
				WaitForMultipleObjects(k - 1, &threads[1], true, INFINITE);
				break;
			}
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
	m_Files.Clear();

	Param.op = eto_finished;
	m_callback->GernalCallback_Callback(&Param, 0);
}
//单个文件擦除
void CEreaserThrads::ErasureThreadRun(PERASURE_FILE_DATA pData)
{
	if (m_Abort)
		return;

	CErasure erasure;
	ERASE_CALLBACK_PARAM Param;
	Param.pData = pData;

	CErasureCallbackImpl impl(&Param);
	impl.m_Control = this;

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
	if(Param == 0) //控制线程结束
		m_ControlThread = nullptr;
}

int CEreaserThrads::ArraySortCompare(ERASURE_FILE_DATA ** it1, ERASURE_FILE_DATA ** it2)
{
	int result;
	result = (*it1)->nFileType - (*it2)->nFileType;
	if (result != 0)
		return result;
	result = _tcslen((*it1)->cFileName) - _tcslen((*it2)->cFileName);
	return result;
}

CEreaserThrads::CErasureCallbackImpl::CErasureCallbackImpl(PERASE_CALLBACK_PARAM pData)
{
	m_Data = pData;
	m_Control = nullptr;
}


CEreaserThrads::CErasureCallbackImpl::~CErasureCallbackImpl()
{
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureStart(UINT nStepCount)
{
	m_Data->op = eto_start;
	if (!m_Control->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;

	if (m_Control->m_Abort)
		return false;
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	m_Data->op = eto_completed;
	if (!m_Control->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;

	if (m_Control->m_Abort)
		return false;
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)   //空文件，或文件夹
		return FALSE;

	if (m_Control->m_Abort)
		return false;

	m_Data->op = eto_progress;
	m_Data->nPercent = nCurent * 100 / nMaxCount;
	if (!m_Control->m_callback->GernalCallback_Callback(m_Data, 0))
		return false;
	return true;
}
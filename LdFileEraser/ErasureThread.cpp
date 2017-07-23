#include "stdafx.h"
#include "ErasureThread.h"

CEreaserThrads::CEreaserThrads(IGernalCallback<PERASE_CALLBACK_PARAM>* callback)
{
	m_callback = callback;
	m_Abort = false;
	m_hEvent = nullptr;
	m_nMaxThreadCount = MAX_THREAD_COUNT;
	m_ControlThread = nullptr;
	m_Files = nullptr;
	m_Method = nullptr;
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

void CEreaserThrads::SetEreaureFiles(CTreeNodes<PERASURE_FILE_DATA> * pFiles)
{
	m_Files = pFiles;
}

void CEreaserThrads::SetEreaureMethod(CErasureMethod* pMethod)
{
	m_Method = pMethod;
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

void CEreaserThrads::ReEresareFile(CTreeNodes<PERASURE_FILE_DATA>* files, int* pThreadCount, bool bWait, HANDLE* threads)
{
	for (int i = 0; i < files->GetCount(); i++)
	{
		if (m_Abort)
			break;

		if (bWait)  //线程已满等待其中一个结束
		{
			int n = WaitForThread(threads);
			if (n == 0) //停止
				break;
			else if (n > m_nMaxThreadCount) //错误
				break;
			else
				*pThreadCount = n - 1;
		}

		CTreeNode<PERASURE_FILE_DATA>* node = files->Get(i);
		if (node->Get()->nStatus != efs_ready && node->Get()->nStatus != efs_abort)
			continue;

		if (node->GetCount())
		{
			ReEresareFile(node->GetItems(), pThreadCount, bWait, threads);
			if (bWait)  //线程已满等待其中一个结束
			{
				int n = WaitForThread(threads);
				if (n == 0) //停止
					break;
				else if (n > m_nMaxThreadCount) //错误
					break;
				else
					*pThreadCount = n - 1;
			}
		}
			
		PERASURE_FILE_DATA pinfo = files->GetValue(i);
		if (pinfo->nStatus == efs_erasured)
			continue;
		CThread* thread = new CThread(this);

		threads[++(*pThreadCount)] = thread->Start((UINT_PTR)pinfo);
		if (!bWait)
			bWait = (*pThreadCount) >= m_nMaxThreadCount;
	}
}

//擦除控制线程
void CEreaserThrads::ControlThreadRun()
{
	ERASE_CALLBACK_PARAM Param;
	ZeroMemory(&Param, sizeof(ERASE_CALLBACK_PARAM));
	Param.op = eto_begin;
	m_callback->GernalCallback_Callback(&Param, 0);

	//保持当前擦除线程
	HANDLE* threads = new HANDLE[m_nMaxThreadCount + 1];             
	ZeroMemory(threads, sizeof(HANDLE)*(m_nMaxThreadCount + 1));
	threads[0] = m_hEvent;
	int k = 0;

	ReEresareFile(m_Files, &k, false, threads);

	delete [] threads;

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
		pData->nErrorCode = erasure.FileErasure(pData->cFileName, m_Method, &impl);
		break;
	case eft_volume:
		pData->nErrorCode = erasure.UnuseSpaceErasure(pData->cFileName, m_Method, &impl);
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

int CEreaserThrads::WaitForThread(HANDLE* threads)
{
	int k = m_nMaxThreadCount + 1;

	DWORD obj = WaitForMultipleObjects(m_nMaxThreadCount + 1, threads, FALSE, INFINITE);
	if (obj == WAIT_FAILED)
	{
		DWORD dw = GetLastError();
		switch (dw)
		{
		case ERROR_ACCESS_DENIED: //不明原因
		case ERROR_INVALID_HANDLE: //擦除已经完成，Handle无效了
			for (int j = 1; j <= m_nMaxThreadCount; j++)
			{//找到那个无线的Thread
				if (GetExitCodeThread(threads[j], &dw))
					continue;
				dw = 0;
				dw = GetLastError();
				if (dw == ERROR_INVALID_HANDLE || dw == ERROR_ACCESS_DENIED)
				{
					k = j;
					break;
				}
			}
			break;
		default:
			DebugOutput(L"错误 %d", dw);
			break;
		}
	}
	else if (obj == WAIT_OBJECT_0)
	{   //终止擦除，等待其他线程都结束。

		for (int j = 1; j <= m_nMaxThreadCount; j++)
		{//找到那个无线的Thread
			WaitForSingleObject(&threads[j], INFINITE);
		}

		k = 0;
	}
	else
		k = obj - WAIT_OBJECT_0; 
	return k;
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
	if(dwErroCode!=0)
		DebugOutput(L"ErasureCompleted %d=%s\n", dwErroCode, m_Data->pData->cFileName);
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
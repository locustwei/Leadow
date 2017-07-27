#include "stdafx.h"
#include "ErasureThread.h"

CEreaserThrads::CEreaserThrads(IEraserThreadCallback* callback)
{
	m_callback = callback;
	m_Abort = false;
	m_hEvent = nullptr;
	m_nMaxThreadCount = 1;
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

void CEreaserThrads::SetEreaureFiles(CLdArray<CFileInfo*> * pFiles)
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

bool CEreaserThrads::ReEresareFile(CLdArray<CFileInfo*>* files, int* pThreadCount, bool& bWait, HANDLE* threads)
{
	for (int i = 0; i < files->GetCount(); i++)
	{
		if (m_Abort)
			return false;

		if (bWait)  //线程已满等待其中一个结束
		{
			int n = WaitForThread(threads);
			if (n == 0) //停止
				return false;
			else if (n > m_nMaxThreadCount) //错误
				return false;
			else
				*pThreadCount = n - 1;
		}

		CFileInfo* file = files->Get(i);

		if (file->IsDirectory())
		{
			//目录，先递归擦除子目录文件
			if (!ReEresareFile(file->GetFiles(), pThreadCount, bWait, threads))
				return false;

			if (m_Abort)
				return false;

			if (bWait)  //线程已满等待其中一个结束
			{
				int n = WaitForThread(threads);
				if (n == 0) //停止
					return false;
				else if (n > m_nMaxThreadCount) //错误
					return false;
				else
					*pThreadCount = n - 1;
			}
		}

		CThread* thread = new CThread(this);
		threads[++(*pThreadCount)] = thread->Start((UINT_PTR)file);
		if (!bWait)
			bWait = (*pThreadCount) >= m_nMaxThreadCount;
	}
	//等等当前目录的擦除线程都结束，以防擦除目录时出错。
	for (int j = 1; j <= m_nMaxThreadCount; j++)
	{
		WaitForSingleObject(&threads[j], INFINITE);
	}
	return true;
}

//擦除控制线程
void CEreaserThrads::ControlThreadRun()
{
	m_callback->EraserThreadCallback(nullptr, eto_start, 0);

	//保持当前擦除线程
	HANDLE* threads = new HANDLE[m_nMaxThreadCount + 1];             
	ZeroMemory(threads, sizeof(HANDLE)*(m_nMaxThreadCount + 1));
	threads[0] = m_hEvent;
	int k = 0;
	bool bWait = false;
	ReEresareFile(m_Files, &k, bWait, threads);

	delete [] threads;

	m_callback->EraserThreadCallback(nullptr, eto_finished, 0);
}
//单个文件擦除
void CEreaserThrads::ErasureThreadRun(CFileInfo* pFile)
{
	if (m_Abort)
		return;
	
	CErasure erasure;

	CErasureCallbackImpl impl(pFile);
	impl.m_Control = this;

	erasure.FileErasure(pFile->GetFullName(), m_Method, &impl);
}

void CEreaserThrads::ThreadRun(CThread * Sender, UINT_PTR Param)
{
	if (Param == 0)
		ControlThreadRun();
	else
		ErasureThreadRun((CFileInfo*)Param);
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
			{//找到那个无效的Thread
				obj = WaitForSingleObject(threads[j], 1);
				if (obj == WAIT_FAILED)
				{
					dw = GetLastError();
					if (dw == ERROR_INVALID_HANDLE || dw == ERROR_ACCESS_DENIED)
					{
						k = j;
						break;
					}
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

CEreaserThrads::CErasureCallbackImpl::CErasureCallbackImpl(CFileInfo* pFile)
{
	m_File = pFile;
	m_Control = nullptr;
}


CEreaserThrads::CErasureCallbackImpl::~CErasureCallbackImpl()
{
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureStart(UINT nStepCount)
{
	if (!m_Control->m_callback->EraserThreadCallback(m_File, eto_begin, nStepCount))
		return false;

	if (m_Control->m_Abort)
		return false;
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureCompleted(UINT nStep, DWORD dwErroCode)
{
	if (!m_Control->m_callback->EraserThreadCallback(m_File, eto_completed, dwErroCode))
		return false;

	if (m_Control->m_Abort)
		return false;

	if(dwErroCode!=0)
		DebugOutput(L"ErasureCompleted %d=%s\n", dwErroCode, m_File->GetFullName());
	return true;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)   //空文件，或文件夹
		return FALSE;

	if (m_Control->m_Abort)
		return false;

	if (!m_Control->m_callback->EraserThreadCallback(m_File, eto_progress, nCurent * 100 / nMaxCount))
		return false;
	return true;
}
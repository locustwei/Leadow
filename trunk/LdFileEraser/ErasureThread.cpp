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

void CEreaserThrads::SetEreaureFiles(CLdArray<CVirtualFile*> * pFiles)
{
	m_Files = pFiles;
}

void CEreaserThrads::SetEreaureMethod(CErasureMethod* pMethod)
{
	m_Method = pMethod;
}

DWORD CEreaserThrads::StartEreasure(UINT nMaxCount)
{
	if (m_ControlThread)  //��һ�β�����û�н���
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

bool CEreaserThrads::ReEresareFile(CLdArray<CVirtualFile*>* files, int* pThreadCount, bool& bWait, HANDLE* threads)
{
	for (int i = 0; i < files->GetCount(); i++)
	{
		if (m_Abort)
			return false;
		CVirtualFile* file = files->Get(i);
		if (file->GetFileType()==vft_folder)
		{
			//Ŀ¼���ȵݹ������Ŀ¼�ļ�
			if (!ReEresareFile(file->GetFiles(), pThreadCount, bWait, threads))
				return false;
		}

		if (bWait)  //�߳������ȴ�����һ������
		{
			int n = WaitForThread(threads);
			if (n == 0) //ֹͣ
				break;
			else if (n > m_nMaxThreadCount) //����
				return false;
			else
				*pThreadCount = n - 1;
		}

		if (m_Abort)
			return false;

		CThread* thread = new CThread(this);
		threads[++(*pThreadCount)] = thread->Start((UINT_PTR)file);
		if (!bWait)
			bWait = (*pThreadCount) >= m_nMaxThreadCount;
	}
	//�ȵȵ�ǰĿ¼�Ĳ����̶߳��������Է�����Ŀ¼ʱ����
	for (int j = 1; j <= m_nMaxThreadCount; j++)
	{
		while(WaitForSingleObject(threads[j], 1000) == WAIT_TIMEOUT)
		{
			DebugOutput(L"�߳̿���\n");  //����ʹ�á�INFINITE���ؿ�����
			if (m_Abort)
				return false;
		}
	}
	if (m_Abort)
		return false;
	return true;
}

//���������߳�
void CEreaserThrads::ControlThreadRun()
{
	m_callback->EraserThreadCallback(nullptr, eto_start, 0);

	//���ֵ�ǰ�����߳�
	HANDLE* threads = new HANDLE[m_nMaxThreadCount + 1];             
	ZeroMemory(threads, sizeof(HANDLE)*(m_nMaxThreadCount + 1));
	threads[0] = m_hEvent;
	int k = 0;
	bool bWait = false;
	ReEresareFile(m_Files, &k, bWait, threads);

	delete [] threads;

	m_callback->EraserThreadCallback(nullptr, eto_finished, 0);
}
//�����ļ�����
void CEreaserThrads::ErasureThreadRun(CVirtualFile* pFile)
{
	if (m_Abort)
		return;
	
	CErasure erasure;

	CErasureCallbackImpl impl(pFile);
	impl.m_Control = this;
	try
	{
		switch (pFile->GetFileType())
		{
		case vft_file:
		case vft_folder:
			erasure.FileErasure(pFile->GetFullName(), m_Method, &impl);
			break;
		case vft_volume:
			erasure.UnuseSpaceErasure((CVolumeInfo*)pFile, m_Method, &impl);
			break;
		default:
			break;
		}
		
		
	}
	catch(...)
	{
		DebugOutput(L"�쳣��\n");
	}
}

void CEreaserThrads::ThreadRun(CThread * Sender, UINT_PTR Param)
{
	if (Param == 0)
		ControlThreadRun();
	else
		ErasureThreadRun((CVirtualFile*)Param);
}

void CEreaserThrads::OnThreadInit(CThread * Sender, UINT_PTR Param)
{
	
}

void CEreaserThrads::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
{
	if(Param == 0) //�����߳̽���
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
		case ERROR_ACCESS_DENIED: //����ԭ��
		case ERROR_INVALID_HANDLE: //�����Ѿ���ɣ�Handle��Ч��
			for (int j = 1; j <= m_nMaxThreadCount; j++)
			{//�ҵ��Ǹ���Ч��Thread
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
			DebugOutput(L"���� %d\n", dw);
			break;
		}
	}
	else if (obj == WAIT_OBJECT_0)
	{   //��ֹ�������ȴ������̶߳�������
		k = 0;
	}
	else
		k = obj - WAIT_OBJECT_0; 

	return k;
}

CEreaserThrads::CErasureCallbackImpl::CErasureCallbackImpl(CVirtualFile* pFile)
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
	if (nMaxCount <= 0)   //���ļ������ļ���
		return FALSE;

	if (m_Control->m_Abort)
		return false;

	if (!m_Control->m_callback->EraserThreadCallback(m_File, eto_progress, nCurent * 100 / nMaxCount))
		return false;
	return true;
}
#include "stdafx.h"
#include "ErasureThread.h"

CEreaserThrads::CEreaserThrads(IEraserThreadCallback* callback)
{
	m_callback = callback;
	m_Abort = false;
	m_hEvent = nullptr;
	m_MaxThreadCount = 1;
	m_ControlThread = nullptr;
	m_Files = nullptr;
	m_Method = nullptr;
	m_ThreadCount = 0;
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

	m_MaxThreadCount = nMaxCount;
	m_ControlThread = new CThread(this);
	m_ControlThread->Start(0);
	return 0;
}

DWORD CEreaserThrads::StartAnalysis(UINT nMaxCount)
{
	if (m_ControlThread)  //��һ�β�����û�н���
		return (DWORD)-1;

	if (m_hEvent == NULL)
		m_hEvent = CreateEvent(nullptr, true, false, nullptr);
	else
		ResetEvent(m_hEvent);

	m_Abort = false;

	m_MaxThreadCount = nMaxCount;
	m_ControlThread = new CThread(this);
	m_ControlThread->Start(1);

	return 0;
}

//�ݹ��ļ��У�Ϊÿ���ļ����������̡߳�
bool CEreaserThrads::ReEresareFile(CLdArray<CVirtualFile*>* files)
{
	LONG volatile* pCount = (LONG volatile*)GlobalAlloc(GPTR, sizeof(LONG));
	*pCount = 0;

	for (int i = 0; i < files->GetCount(); i++)
	{
		if (m_Abort)
			return false;
		CVirtualFile* file = files->Get(i);
		if (file->GetFileType()==vft_folder)
		{
			//Ŀ¼���ȵݹ������Ŀ¼�ļ�
			if (!ReEresareFile(file->GetFiles()/*, pThreadCount, bWait, threads*/))
				return false;
		}

		int n = WaitForThread();
		if (n == 0) //ֹͣ
			break;

		if (m_Abort)
			return false;

		CThread* thread = new CThread(this);
		thread->SetTag((UINT_PTR)pCount);
		thread->Start((UINT_PTR)file);
	}
	//�ȵȵ�ǰĿ¼�Ĳ����̶߳��������Է�����Ŀ¼ʱ����
	while(*pCount>0)
	{
		Sleep(20);
	}
	GlobalFree((HGLOBAL)pCount);
	if (m_Abort)
		return false;
	return true;
}

//���������߳�,���������ļ������̣߳������ļ������߳�����
void CEreaserThrads::ControlThreadRun(UINT_PTR Param)
{
	m_ThreadCount = 0;
	if (Param == 0)
	{
		m_callback->EraserThreadCallback(nullptr, eto_start, 0);
		ReEresareFile(m_Files);
		m_callback->EraserThreadCallback(nullptr, eto_finished, 0);
	}else
	{
		m_callback->EraserThreadCallback(nullptr, eto_start, 0);
		for (int i = 0; i < m_Files->GetCount(); i++)
		{
			if (m_Abort)
				return ;
			CVirtualFile* file = m_Files->Get(i);
			if (file->GetFileType() == vft_volume)
			{
				CThread* thread = new CThread(this);
				thread->SetTag((UINT_PTR)pCount);
				thread->Start((UINT_PTR)file);
			}
		}
		m_callback->EraserThreadCallback(nullptr, eto_finished, 0);
	}
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


void CEreaserThrads::ThreadBody(CThread * Sender, UINT_PTR Param)
{
	if (Param == 0 || //�ļ������߳�
		Param == 1)   //���̷����߳�
		ControlThreadRun(Param);
	else
	{
		InterlockedIncrement(&m_ThreadCount);
		LONG volatile* pCount = (LONG volatile*)Sender->GetTag();
		InterlockedIncrement(pCount);
		ErasureThreadRun((CVirtualFile*)Param);
	}
}

void CEreaserThrads::OnThreadInit(CThread * Sender, UINT_PTR Param)
{
	
}

void CEreaserThrads::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
{
	if(Param == 0) //�����߳̽���
		m_ControlThread = nullptr;
	else
	{
		InterlockedDecrement(&m_ThreadCount);
		LONG volatile* pCount = (LONG volatile*)Sender->GetTag();
		InterlockedDecrement(pCount);
	}
}

int CEreaserThrads::WaitForThread(/*HANDLE* threads*/)
{
	int k = 1;

	while(m_ThreadCount >= m_MaxThreadCount)
	{
		if(WaitForSingleObject(m_hEvent, 10)==WAIT_OBJECT_0)
		{
			k = 0;
			break;
		};
	}

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

	if (!m_Control->m_callback->EraserThreadCallback(m_File, eto_progress, (DWORD)(nCurent * 100 / nMaxCount)))
		return false;
	return true;
}
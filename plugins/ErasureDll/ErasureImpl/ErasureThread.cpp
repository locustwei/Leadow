#include "stdafx.h"
#include "ErasureThread.h"
#include "EraseTest.h"

CEreaserThrads::CEreaserThrads()
{
	m_callback = nullptr;
	m_Abort = false;
	m_hEvent = nullptr;
	m_MaxThreadCount = 1;
	m_ControlThread = nullptr;
	m_Files = nullptr;
	m_FileMothed = nullptr;
	m_VolumeMothed = nullptr;
	m_ThreadCount = 0;
	ZeroMemory(&m_Options, sizeof(ERASER_OPTIONS));
	m_Options.bRemoveFolder = true;
	m_Options.bSkipSpace = false;
	m_Options.bSkipTrack = false;
	m_Options.bFreeFileSpace = false;
	m_Options.FileMothed = em_DoD_E;
	m_Options.VolumeMothed = em_Pseudorandom;
}

CEreaserThrads::~CEreaserThrads()
{
	StopThreads();

	if(m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}
	if (m_FileMothed)
		delete m_FileMothed;
	if (m_VolumeMothed)
		delete m_VolumeMothed;
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

DWORD CEreaserThrads::StartEreasure(UINT nMaxCount)
{
	if (m_ControlThread)  //上一次擦除还没有结束
		return (DWORD)-1;

	if (m_FileMothed)
		delete m_FileMothed;
	if (m_VolumeMothed)
		delete m_VolumeMothed;

	m_FileMothed = new CErasureMothed(m_Options.FileMothed);
	m_VolumeMothed = new CErasureMothed(m_Options.VolumeMothed);

	if (m_hEvent == NULL)
		m_hEvent = CreateEvent(nullptr, true, false, nullptr);
	else
		ResetEvent(m_hEvent);
	
	m_Abort = false;

	m_MaxThreadCount = nMaxCount;
	m_ControlThread = new CThread();
	m_ControlThread->Start(CLdMethodDelegate::MakeDelegate(this, &CEreaserThrads::EraseFile_Thread), 0);
	return 0;
}
//磁盘状态分析
DWORD CEreaserThrads::StartAnalysis(UINT nMaxCount)
{
	if (m_ControlThread)  //上一次擦除还没有结束
		return (DWORD)-1;

	if (m_hEvent == NULL)
		m_hEvent = CreateEvent(nullptr, true, false, nullptr);
	else
		ResetEvent(m_hEvent);

	m_Abort = false;

	m_MaxThreadCount = nMaxCount;
	m_ControlThread = new CThread();
	m_ControlThread->Start(CLdMethodDelegate::MakeDelegate(this, &CEreaserThrads::VolumeAnalyThread), 0);
//	m_ControlThread->Start(1);

	return 0;
}

//递归文件夹，为每个文件创建擦除线程。
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
			//目录，先递归擦除子目录文件
			if (!ReEresareFile(((CFolderInfo*)file)->GetFiles()/*, pThreadCount, bWait, threads*/))
				return false;
		}
		
		//DebugOutput(L"main thread %d start %s\n", m_ThreadCount, file->GetFullName());

		int n = WaitForThread();
		if (n == 0) //停止
			break;

		if (m_Abort)
			return false;

		//LONG nTemp = *pCount;
		CThread* thread = new CThread(this);
		thread->SetTag((UINT_PTR)pCount);
		thread->Start(CLdMethodDelegate::MakeDelegate(this, &CEreaserThrads::ErasureThreadRun), (UINT_PTR)file);
		//while (nTemp == *pCount)  //等待这个擦除线程真正运行，否则在线程还没运行起来又创建了多余的线程。
			//Sleep(10);
	}
	//等等当前目录的擦除线程都结束，以防擦除目录时出错。
	while(*pCount>0)
	{
		Sleep(20);
	}
	GlobalFree((HGLOBAL)pCount);
	if (m_Abort)
		return false;
	return true;
}

//INT_PTR CEreaserThrads::FileAnal_Thread(PVOID, UINT_PTR Param)
//{
//	m_ThreadCount = 0;
//	m_callback->EraserReprotStatus(nullptr, eto_start, 0);
//
//	for (int i = 0; i < m_Files->GetCount(); i++)
//	{
//		if (m_Abort)
//			return 0;
//		if (WaitForThread() == 0)
//			break;
//		CVirtualFile* file = m_Files->Get(i);
//		if (file->GetFileType() == vft_volume)
//		{
//			CThread* thread = new CThread();
//			thread->Start(CLdMethodDelegate::MakeDelegate(this, &CEreaserThrads::VolumeAnalyThread), (UINT_PTR)file);
//		}else
//		{
//			CThread* thread = new CThread();
//			thread->Start(CLdMethodDelegate::MakeDelegate(this, &CEreaserThrads::FileAnalyThread), (UINT_PTR)file);
//		}
//	}
//	while (m_ThreadCount>0)
//	{
//		Sleep(20);
//	}
//	m_callback->EraserReprotStatus(nullptr, eto_finished, 0);
//
//	return 0;
//}

//擦除控制线程,创建单个文件擦除线程，控制文件擦除线程数。
INT_PTR CEreaserThrads::EraseFile_Thread(PVOID, UINT_PTR Param)
{
	m_ThreadCount = 0;
	m_callback->EraserReprotStatus(nullptr, nullptr, eto_start, 0);
	ReEresareFile(m_Files);
	Sleep(10); //防止线程还没开始控制线程就认为所有线程都结束了

	while (m_ThreadCount>0)
	{
		Sleep(20);
	}
	m_callback->EraserReprotStatus(nullptr, nullptr, eto_finished, 0);

	return 0;
}
//单个文件擦除
INT_PTR CEreaserThrads::ErasureThreadRun(PVOID pData, UINT_PTR Param)
{
	if (m_Abort)
		return 0;

	CVirtualFile* pFile = (CVirtualFile*)Param;

	CErasure erasure;

	CErasureCallbackImpl impl(pFile);
	impl.m_Control = this;

	switch (pFile->GetFileType())
	{
	case vft_file:
		erasure.FileErasure(pFile->GetFullName(), m_FileMothed, &impl);
		break;
	case vft_folder:
		if(m_Options.bRemoveFolder)
			erasure.DirectoryErasure(pFile->GetFullName(), &impl);
		break;
	case vft_volume:
		erasure.UnuseSpaceErasure((CVolumeInfo*)pFile, m_VolumeMothed, &impl, m_Options.bSkipSpace, m_Options.bSkipTrack);
		break;
	default:
		break;
	}

	return 0;
}

INT_PTR CEreaserThrads::VolumeAnalyThread(PVOID pData, UINT_PTR Param)
{
	if (m_Abort)
		return 0;

	//CVolumeInfo* pVolume = (CVolumeInfo*)Param;

	//DWORD error;
	//if (!m_callback->EraserReprotStatus(pVolume->GetFullName(), eto_analy, 0))
	//	error = ERROR_CANCELED;
	//else
	//{
	//	CEraseTest Test;
	//	error = Test.TestVolume(pVolume->GetFullName(), nullptr);
	//	//error = pVolume->StatisticsFileStatus();
	//}
	//m_callback->EraserReprotStatus(pVolume->GetFullName(), eto_analied, error);

	return 0;
}

INT_PTR CEreaserThrads::FileAnalyThread(PVOID pData, UINT_PTR Param)
{
	CVirtualFile* pFile = (CVirtualFile*)Param;
	if(pFile->GetFileType()==vft_folder)
	{
		((CFolderInfo*)pFile)->FindFiles(true);
	}
	return 0;
}

void CEreaserThrads::ThreadBody(CThread * Sender, UINT_PTR Param)
{
	
}

void CEreaserThrads::OnThreadInit(CThread * Sender, UINT_PTR Param)
{
	if (Param != 0 && //文件擦除线程
		Param != 1)
	{
		InterlockedIncrement(&m_ThreadCount);
		LONG volatile* pCount = (LONG volatile*)Sender->GetTag();
		if (pCount)  //这是擦除线程
		{
			InterlockedIncrement(pCount);
		}
	}
}

void CEreaserThrads::OnThreadTerminated(CThread * Sender, UINT_PTR Param)
{
	if(Param == 0 || //文件擦除线程
		Param == 1)   //磁盘分析线程
		m_ControlThread = nullptr;
	else
	{
		LONG volatile* pCount = (LONG volatile*)Sender->GetTag();
		if (pCount)
		{
			InterlockedDecrement(pCount);
			//DebugOutput(L"end ------ %d %d %s\n", m_ThreadCount, *pCount, ((CVirtualFile*)Param)->GetFullName());
		}
		InterlockedDecrement(&m_ThreadCount);
	}
}
//等待其中一个线程结束，或者取消操作
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

PERASER_OPTIONS CEreaserThrads::GetOptions()
{
	return &m_Options;
}

void CEreaserThrads::SetCallback(IEraserListen* callback)
{
	m_callback = callback;
}

CEreaserThrads::CErasureCallbackImpl::CErasureCallbackImpl(CVirtualFile* pFile)
{
	m_File = pFile;
	CVirtualFile* p = m_File->GetFolder();
	while (p && p->GetFolder() != nullptr)
		p = p->GetFolder();
	m_Folder = p;
	m_Control = nullptr;
}


CEreaserThrads::CErasureCallbackImpl::~CErasureCallbackImpl()
{
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureStart()
{

	m_Control->m_Abort = !m_Control->m_callback->EraserReprotStatus(m_Folder == nullptr ? m_File->GetFullName() : m_Folder->GetFullName(), m_Folder == nullptr ? nullptr : m_File->GetFullName(), eto_begin, 0);
	return !m_Control->m_Abort;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureCompleted(DWORD dwErroCode)
{
	if (dwErroCode == 0)
	{
		m_Control->m_Abort = !m_Control->m_callback->EraserReprotStatus(m_Folder==nullptr? m_File->GetFullName():m_Folder->GetFullName(), m_Folder==nullptr?nullptr:m_File->GetFullName(), eto_completed, dwErroCode);
	}
	else
	{
		m_Control->m_Abort = !m_Control->m_callback->EraserReprotStatus(m_Folder == nullptr ? m_File->GetFullName() : m_Folder->GetFullName(), m_Folder == nullptr ? nullptr : m_File->GetFullName(), eto_error, dwErroCode);
		DebugOutput(L"ErasureCompleted %s %d", m_File->GetFileName(), dwErroCode);
	}

	if (m_Folder)  //文件夹下的文件，更新顶层文件夹的进度
	{

		PFILE_ERASURE_DATA pEraserData = (PFILE_ERASURE_DATA)(m_Folder->GetTag());
		if (pEraserData && dwErroCode == 0)
			InterlockedIncrement(&pEraserData->nErasued);

		int percent = 100;

		//更新所属文件夹进度
		if (pEraserData->nErasued < pEraserData->nCount)
		{
			percent = (int)ceil(pEraserData->nErasued * 100 / pEraserData->nCount);
		}

		m_Control->m_Abort = !m_Control->m_callback->EraserReprotStatus(m_Folder->GetFullName(),nullptr, eto_progress, percent);
	}

	return !m_Control->m_Abort;
}

BOOL CEreaserThrads::CErasureCallbackImpl::ErasureProgress(ERASE_STEP nStep, UINT64 nMaxCount, UINT64 nCurent)
{
	if (nMaxCount <= 0)   //空文件，或文件夹
		return FALSE;

	E_THREAD_OPTION option;

	switch(nStep)
	{
	//case ERASER_MFT_FREE:  //归集到ERASER_DATA_FREE的一部分
	case ERASER_DATA_FREE:
		option = eto_freespace;
		break;
	case ERASER_DEL_TEMPFILE:
	case ERASER_FILE:
		option = eto_progress;
		break;
	case ERASER_DEL_TRACK: 
		option = eto_track;
		break;
	default:
		option = eto_progress;
		break;
	}

	m_Control->m_Abort = !m_Control->m_callback->EraserReprotStatus(m_Folder==nullptr?m_File->GetFullName():m_Folder->GetFullName(), m_Folder==nullptr?m_File->GetFullName():nullptr, option, (DWORD)ceil(nCurent * 100 / nMaxCount));

	return !m_Control->m_Abort;
}
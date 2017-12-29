#include "stdafx.h"
#include "LdString.h"
#include "ShareData.h"

namespace LeadowLib
{
	CShareData::CShareData(TCHAR* pName)
		: m_hFile(nullptr)
		, m_pFileHeader(nullptr)
		, m_Size(0)
		, m_Name()
		, m_TermateReadThread(true)
		, m_nTimeOut(INFINITE)
	{
		m_Name = pName;
		m_hSemaphore = CreateSemaphore(nullptr, 0, 1, pName);
		m_IsMaster = GetLastError() == 0;// ERROR_ALREADY_EXISTS;
	
		CLdString s = pName;
		s += _T("master");
		HANDLE hMaster = CreateEvent(nullptr, TRUE, TRUE, s);
		s = pName;
		s += _T("client");
		HANDLE hClient = CreateEvent(nullptr, TRUE, TRUE, s);
		
		m_hWaitEvent = m_IsMaster ? hClient : hMaster;
		m_hSetEvent = m_IsMaster ? hMaster : hClient;
	}

	void CShareData::ThreadBody(CThread* Sender, UINT_PTR Param)
	{
		m_TermateReadThread = false;
		IGernalCallback<PVOID>* ReadCallback = (IGernalCallback<PVOID>*)Param;
		while (!m_TermateReadThread)
		{
			DWORD k = WaitForSingleObject(m_hWaitEvent, INFINITE);
			
			if (k == WAIT_OBJECT_0)
			{
				WORD nSize = m_pFileHeader->nSize;
				BYTE* Data = new BYTE[nSize];
				Read(Data, nSize);
				if (ReadCallback)
					ReadCallback->GernalCallback_Callback(Data, nSize);
				delete[] Data;
			}
			else
				break;
		}
		m_TermateReadThread = true;
	}

	void CShareData::OnThreadInit(CThread* Sender, UINT_PTR Param)
	{
	}

	void CShareData::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
	{
	}

	CShareData::~CShareData()
	{
		if (m_pFileHeader)
			UnmapViewOfFile(m_pFileHeader);
		if (m_hFile)
			CloseHandle(m_hFile);
		if (m_hSemaphore)
			CloseHandle(m_hSemaphore);
		if (m_hWaitEvent)
			CloseHandle(m_hWaitEvent);
		if (m_hSetEvent)
			CloseHandle(m_hSetEvent);
	}

	DWORD CShareData::Write(PVOID pData, UINT nLength)
	{
		if (!m_hFile || !m_pFileHeader || nLength > m_Size)
			return DWORD(-1);
		if(WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
			return DWORD(-1);
		m_pFileHeader->nSign = 0;
		m_pFileHeader->nSize = nLength;
		MoveMemory(m_pFileHeader->Data, pData, nLength);
		FlushViewOfFile(m_pFileHeader, m_Size);
		ReleaseSemaphore(m_hSemaphore, 1, nullptr);
		::PulseEvent(m_hSetEvent);
		return 0;
	}

	DWORD CShareData::Read(PVOID pData, UINT nLength)
	{
		if (!m_hFile || !m_pFileHeader ||  nLength > m_Size)
			return DWORD(-1);
		//等写入数据
		if(WaitForSingleObject(m_hWaitEvent, m_nTimeOut) != WAIT_OBJECT_0)
			return (DWORD)-1;
		//进临界区
		if(WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
			return (DWORD)-1;
		MoveMemory(pData, m_pFileHeader->Data, nLength);
		m_pFileHeader->nSign = 1;
		FlushViewOfFile(m_pFileHeader, sizeof(struct MAP_DATA));
		ReleaseSemaphore(m_hSemaphore, 1, nullptr);
		return 0;
	}

	DWORD CShareData::StartReadThread(IGernalCallback<PVOID>* ReadCallback)
	{
		CThread* thread = new CThread(this);
		thread->Start((UINT_PTR)ReadCallback);
		return 0;
	}

	void CShareData::StopReadThread()
	{
		if (!m_TermateReadThread)
		{
			m_TermateReadThread = true;
			ResetEvent(m_hWaitEvent);
		}
	}

	void CShareData::SetWaitTimeOut(UINT nTime)
	{
		m_nTimeOut = nTime;
	}

	CShareData* CShareData::Create(TCHAR* pName, UINT nSize)
	{
		nSize += sizeof(struct MAP_DATA);

		HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, nSize, pName);
		if (hMap == nullptr)
			return nullptr;
		CShareData* result = new CShareData(pName);
		result->m_hFile = hMap;
		result->m_pFileHeader = (struct MAP_DATA*)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, nSize);
		result->m_Size = nSize;
		return result;
	}
}
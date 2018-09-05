#include "stdafx.h"
#include "LdString.h"
#include "ShareData.h"
#include "../LdLib.h"

namespace LeadowLib
{
	CShareData::CShareData(TCHAR* pName, DWORD nSize)
		: m_hFile(nullptr)
		, m_pFileHeader(nullptr)
		, m_Size(0)
		, m_Name()
		, m_TermateReadThread(true)
		, m_nTimeOut(INFINITE)
		, m_FreeOnTerminate(false)
	{
		nSize += sizeof(SHAREDATA_DATA);
		HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, nSize, pName);
		
		if (hMap == nullptr)
			return;
	
//		m_IsMaster = GetLastError() == 0;// ERROR_ALREADY_EXISTS;
		m_hFile = hMap;
		m_pFileHeader = (PSHAREDATA_DATA)MapViewOfFile(hMap, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, nSize);
		m_ThisID = *(PWORD)m_pFileHeader;   //用户编号
		*(PWORD)m_pFileHeader = m_ThisID + 1; 
		m_pFileHeader = (PSHAREDATA_DATA)((PBYTE)m_pFileHeader + sizeof(DWORD));
		m_Size = nSize;

		m_Name = pName;
		CLdString s = pName;

#ifdef _DEBUG
		InitializeCriticalSection(&m_CriticalSection);
#else
		s += _T("samaphore");
		m_hSemaphore = CreateSemaphore(nullptr, 1, 1, s);
		if (!m_hSemaphore)
			DebugOutput(L"CreateSemaphore error = %d\n", GetLastError());
#endif // _DEBUG

		s = pName;
		s += _T("event");
		m_hReadEvent = CreateEvent(nullptr, TRUE, FALSE, s);
		if (!m_hReadEvent)
			DebugOutput(L"CreateEvent error = %d\n", GetLastError());
	}

	void CShareData::ThreadBody(CThread* Sender, UINT_PTR Param)
	{
		m_TermateReadThread = false;
		while (!m_TermateReadThread)
		{
			if (WaitForSingleObject(m_hReadEvent, m_nTimeOut) != WAIT_OBJECT_0)
				return ;
			//进临界区
#ifdef _DEBUG
			EnterCriticalSection(&m_CriticalSection);
#else
			if (WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
				return;
#endif // _DEBUG
			ResetEvent(m_hReadEvent);

			DebugOutput(L"begin CShareData  read...\n");

			__try
			{
				if (m_TermateReadThread)
					break;

				if (m_pFileHeader->nSize)
					if (m_pFileHeader->id != m_ThisID)   //自己写的数据不要读
					{
						DebugOutput(L"read data size=%d\n", m_pFileHeader->nSize);
						WORD nSize = m_pFileHeader->nSize;
						BYTE* pData = new BYTE[m_pFileHeader->nSize];
						::MoveMemory(pData, m_pFileHeader->Data, nSize);
						m_TermateReadThread = !m_ReadCallback(pData, Param);
						delete[] pData;
					}
			}
			__finally
			{
#ifdef _DEBUG
				LeaveCriticalSection(&m_CriticalSection);
#else
				ReleaseSemaphore(m_hSemaphore, 1, nullptr);
#endif // _DEBUG

			}

		}
		m_TermateReadThread = true;
		DebugOutput(L"CShareData::ThreadBody out\n");
	}

	void CShareData::OnThreadInit(CThread* Sender, UINT_PTR Param)
	{
	}

	void CShareData::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
	{
		if (m_FreeOnTerminate)
			delete this;
	}

	CShareData::~CShareData()
	{
		if (m_pFileHeader)
			UnmapViewOfFile(m_pFileHeader);
		if (m_hFile)
			CloseHandle(m_hFile);
#ifdef _DEBUG
		DeleteCriticalSection(&m_CriticalSection);
#else
		if (m_hSemaphore)
			CloseHandle(m_hSemaphore);
#endif // _DEBUG
		if (m_hReadEvent)
			CloseHandle(m_hReadEvent);
//		if (m_hWriteEvent)
//			CloseHandle(m_hWriteEvent);
	}

	CLdString& CShareData::GetName()
	{
		return m_Name;
	}

	DWORD CShareData::Write(PVOID pData, UINT nLength)
	{
		if (!m_hFile || !m_pFileHeader || nLength > m_Size)
			return DWORD(-1);
#ifdef _DEBUG
		EnterCriticalSection(&m_CriticalSection);
#else
		if(WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
			return GetLastError();
#endif // _DEBUG
		__try
		{
			DebugOutput(L"write data size=%d\n", nLength);

			m_pFileHeader->id = m_ThisID;
			m_pFileHeader->nSize = nLength;
			MoveMemory(m_pFileHeader->Data, pData, nLength);
			FlushViewOfFile(m_pFileHeader, m_Size);
			::SetEvent(m_hReadEvent);
#ifdef _DEBUG
			LeaveCriticalSection(&m_CriticalSection);
#else
			ReleaseSemaphore(m_hSemaphore, 1, nullptr);
#endif // _DEBUG
		}
		__finally
		{
		}
		return 0;
	}

	//DWORD CShareData::Read(PBYTE* pData, WORD* nLength)
	//{
	//	if (!m_hFile || !m_pFileHeader || !nLength || !pData)
	//		return DWORD(-1);
	//	//等写入数据
	//	if (WaitForSingleObject(m_hReadEvent, m_nTimeOut) != WAIT_OBJECT_0)
	//		return GetLastError();
	//	//进临界区
	//	if (WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
	//		return GetLastError();
	//	__try
	//	{
	//		if (m_pFileHeader->nSize) 
	//			if(m_pFileHeader->id != m_ThisID)   //自己写的数据不要读
	//			{
	//				DebugOutput(L"read data size=%d\n", m_pFileHeader->nSize);
	//				*nLength = m_pFileHeader->nSize;
	//				*pData = new BYTE[m_pFileHeader->nSize];
	//				MoveMemory(*pData, m_pFileHeader->Data, *nLength);
	//				//m_pFileHeader->nSize = 0;
	//				return 0;
	//			}
	//	}
	//	__finally
	//	{
	//		ReleaseSemaphore(m_hSemaphore, 1, nullptr);
	//	}
	//	return 0;
	//}

	DWORD CShareData::StartReadThread(CLdMethodDelegate ReadCallback, UINT_PTR Param, bool FreeOnTerminate)
	{
		m_FreeOnTerminate = FreeOnTerminate;
		m_ReadCallback = ReadCallback;
		CThread* thread = new CThread(this);
		thread->Start(Param);
		return 0;
	}

	void CShareData::StopReadThread()
	{
		if (!m_TermateReadThread)
		{
			m_TermateReadThread = true;
			ResetEvent(m_hReadEvent);
		}
	}

	void CShareData::SetWaitTimeOut(UINT nTime)
	{
		m_nTimeOut = nTime;
	}

}

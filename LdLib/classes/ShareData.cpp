#include "stdafx.h"
#include "LdString.h"
#include "ShareData.h"
#include "../../LdApp/LdApp.h"

namespace LeadowLib
{
	CShareData::CShareData(TCHAR* pName, WORD nSize)
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
		m_ThisID = *(PWORD)m_pFileHeader;   //�û����
		*(PWORD)m_pFileHeader = m_ThisID + 1; 
		m_pFileHeader = (PSHAREDATA_DATA)((PBYTE)m_pFileHeader + sizeof(DWORD));
		m_Size = nSize;

		m_Name = pName;

		CLdString s = pName;
		s += _T("samaphore");
		m_hSemaphore = CreateSemaphore(nullptr, 1, 1, s);
		if (!m_hSemaphore)
			DebugOutput(L"CreateSemaphore error = %d", GetLastError());

		s = pName;
		s += _T("event");
		m_hReadEvent = CreateEvent(nullptr, TRUE, FALSE, s);
		if (!m_hReadEvent)
			DebugOutput(L"CreateEvent error = %d", GetLastError());
//		s = pName;
//		s += _T("client");
//		HANDLE hClient = CreateEvent(nullptr, TRUE, TRUE, s);
		
//		m_hReadEvent = m_IsMaster ? hClient : hMaster;
		//m_hWriteEvent = m_IsMaster ? hMaster : hClient;
	}

	void CShareData::ThreadBody(CThread* Sender, UINT_PTR Param)
	{
		m_TermateReadThread = false;
		while (!m_TermateReadThread)
		{
			DebugOutput(L"CShareData Wait For read...");
//			DWORD k = WaitForSingleObject(m_hReadEvent, INFINITE);
//			
//			if (k == WAIT_OBJECT_0)
			{
				WORD nSize;
				BYTE* Data;
				if (Read(&Data, &nSize) == 0)
				{
					m_TermateReadThread = !m_ReadCallback(Data, nSize);
					delete[] Data;
				}
			}
//			else
//				break;
		}
		m_TermateReadThread = true;
		DebugOutput(L"CShareData::ThreadBody out");
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
		if (m_hSemaphore)
			CloseHandle(m_hSemaphore);
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
		if(WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
			return GetLastError();
		
		DebugOutput(L"write data size=%d", nLength);
		
		m_pFileHeader->id = m_ThisID;
		m_pFileHeader->nSize = nLength;
		MoveMemory(m_pFileHeader->Data, pData, nLength);
		FlushViewOfFile(m_pFileHeader, m_Size);
		ReleaseSemaphore(m_hSemaphore, 1, nullptr);
		::PulseEvent(m_hReadEvent);
		return 0;
	}

	DWORD CShareData::Read(PBYTE* pData, WORD* nLength)
	{
		if (!m_hFile || !m_pFileHeader || !nLength || !pData)
			return DWORD(-1);
		//��д������
		if (WaitForSingleObject(m_hReadEvent, m_nTimeOut) != WAIT_OBJECT_0)
			return GetLastError();
		//���ٽ���
		if (WaitForSingleObject(m_hSemaphore, m_nTimeOut) != WAIT_OBJECT_0)
			return GetLastError();
		__try
		{
			if (m_pFileHeader->nSize) 
				if(m_pFileHeader->id != m_ThisID)   //�Լ�д�����ݲ�Ҫ��
				{
					DebugOutput(L"read data size=%d", m_pFileHeader->nSize);
					*nLength = m_pFileHeader->nSize;
					*pData = new BYTE[m_pFileHeader->nSize];
					MoveMemory(*pData, m_pFileHeader->Data, *nLength);
					//m_pFileHeader->nSize = 0;
					return 0;
				}
		}
		__finally
		{
			ReleaseSemaphore(m_hSemaphore, 1, nullptr);
		}
		return -1;
	}

	DWORD CShareData::StartReadThread(CMethodDelegate ReadCallback, bool FreeOnTerminate)
	{
		m_FreeOnTerminate = FreeOnTerminate;
		m_ReadCallback = ReadCallback;
		CThread* thread = new CThread(this);
		thread->Start(0);
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

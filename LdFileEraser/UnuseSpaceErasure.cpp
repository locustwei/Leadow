#include "stdafx.h"
#include "UnuseSpaceErasure.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp__")
#define MAX_TEMPFILESIZE 1024 * 1024 * 1024

CUnuseSpaceErasure::CUnuseSpaceErasure()
{
}


CUnuseSpaceErasure::~CUnuseSpaceErasure()
{
}

DWORD CUnuseSpaceErasure::Execute(CLdString & Driver, CErasureMethod& method, IErasureCallback* callback)
{

	//检查是否有管理员权限（错误）
	//检查是否有系统还原点（警告）
	//检查磁盘用户配额（警告 GetDiskFreeSpaceEx）
	DWORD Result = 0;

	do
	{
		if(!callback->ErasureStart(3))
			break;
		m_callback = callback;
		m_method = &method;

		Result = m_volInfo.OpenVolumePath(Driver);
		if (Result != 0)
			break;
		DWORD dwAttribute = GetFileAttributes(Driver);  //FILE_ATTRIBUTE_REPARSE_POINT
		BOOL IsCompressed = CFileUtils::IsCompressed(Driver);
		if (IsCompressed)
			CFileUtils::SetCompression(Driver, FALSE);
		m_tmpDir = Driver;
		m_tmpDir += Erasure_temp_path;
		Result = CFileUtils::ForceDirectories(m_tmpDir);
		if (Result != 0)
			break;

		Result = EraseUnusedSpace();
		if(Result != 0)
			break;

	} while (false);
	

	return Result;
}

DWORD CUnuseSpaceErasure::EraseUnusedSpace()
{
	DWORD Result = 0;
	UINT64 nCurrent = 0, nMaxCount = m_volInfo.GetAvailableFreeSpace();
	UINT64 nFileSize;
	m_callback->ErasureProgress(1, nMaxCount, nCurrent);

	UINT nFileCount = nMaxCount / MAX_TEMPFILESIZE + 1;
	while (true)
	{
		TCHAR tmpName[MAX_PATH] = { 0 };
		if (!GetTempFileName(m_tmpDir, _T("__LD"), 0, tmpName))
		{
			Result = GetLastError();
			break;
		}
		HANDLE hFile = CreateFile(tmpName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			Result = GetLastError();
			break;
		}
		nFileSize = min(MAX_TEMPFILESIZE, m_volInfo.GetAvailableFreeSpace());
		FILE_END_OF_FILE_INFO einfo = { 0 };
		einfo.EndOfFile.QuadPart = nFileSize;
		if (!SetFileInformationByHandle(hFile, FileEndOfFileInfo, &einfo, sizeof(FILE_END_OF_FILE_INFO)))
		{
			Result = GetLastError();
			break;
		}
		Result = EraserFile(hFile, 0, nFileSize, NULL);
		if(Result != 0)
			break;
		nCurrent += nFileCount;

		m_callback->ErasureProgress(1, nMaxCount, nCurrent);
	}

	return 0;
}

DWORD CUnuseSpaceErasure::EraserFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	DWORD Result;
	UINT64 nProcessCount = nFileSize * m_method->GetPassCount();
	UINT64 nProcessCurrent = 0;

	class IErasureCallbackImpl: public IErasureCallback
	{
	public:
		IErasureCallbackImpl()
		{
		};
		~IErasureCallbackImpl()
		{
		};


		virtual BOOL ErasureStart(UINT nStepCount) override
		{
			return 0;
		}


		virtual VOID ErasureCompleted(UINT nStep, DWORD dwErroCode) override
		{
		}


		virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override
		{
			nProcessCurrent += nCurent;
			callbck->ErasureProgress(nStep, nProcessCount, nProcessCurrent);
		}

	private:

	};

	IErasureCallbackImpl callimpl;

	callbck->ErasureStart(m_method->GetPassCount());

	for (int i = 0; i < m_method->GetPassCount(); i++)
	{
		ErasureMethodPass* pd = m_method->GetPassData(i);
		switch (pd->function)
		{
		case WriteRandom:
			Result = WriteFileRandom(hFile, nStartPos, nFileSize, &callimpl);
			break;
		case WriteConstant:
			Result = WriteFileConst(hFile, nStartPos, nFileSize, &callimpl, pd->bytes, pd->nCount);
			break;
		}
	}

	return 0;
}

DWORD CUnuseSpaceErasure::WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	if(SetFilePointer(hFile, nStartPos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();

#define MAX_BUFFER_SIZE 1024

	BYTE Buffer[MAX_BUFFER_SIZE];
	DWORD dwCb;
	UINT64 nCurrent = 0;
	while (nFileSize > nCurrent)
	{
		UINT nBufferSize = min(MAX_BUFFER_SIZE, nFileSize);
		for (int i = 0; i < nBufferSize; i++)
			Buffer[i] = rand() & 0xFF;
		
		if (!WriteFile(hFile, Buffer, nBufferSize, &dwCb, NULL))
		{
			return GetLastError();
		}
		nCurrent += nBufferSize;

		callbck->ErasureProgress(1, nFileSize, nCurrent);
	}
	return 0;
}

DWORD CUnuseSpaceErasure::WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount)
{
	if(SetFilePointer(hFile, nStartPos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();
	
	UINT nMaxLength = nCount * 1024;

	PBYTE Buffer = new BYTE[nMaxLength];
	DWORD dwCb, Result = 0;
	UINT64 nCurrent = 0;
	while (nFileSize > nCurrent)
	{
		UINT nBufferSize = min(nMaxLength, nFileSize);

		for (int i = 0; i < nBufferSize / nCount; i++)
			memcpy(Buffer + i*nCount, bytes, nCount);

		if (!WriteFile(hFile, Buffer, nBufferSize, &dwCb, NULL))
		{
			Result = GetLastError();
			break;
		}
		nCurrent += nBufferSize;

		callbck->ErasureProgress(1, nFileSize, nCurrent);
	}
	delete[] Buffer;

	return Result;
}

#include "stdafx.h"
#include "Erasure.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp")
#define MAX_TEMPFILESIZE 1024 * 1024 * 1024

CErasure::CErasure()
{
}


CErasure::~CErasure()
{
}
/*!
分步进行回掉函数实现
 */
class IErasureCallbackImpl : public IErasureCallback
{
public:
	IErasureCallback* m_Callback;
	UINT64 m_Max, m_Current, m_Step;

	IErasureCallbackImpl()
	{
		m_Current = 0;
		m_Max = 0;
		m_Callback = NULL;
	};
	~IErasureCallbackImpl()
	{
	};


	virtual BOOL ErasureStart(UINT nStepCount) override
	{
		return TRUE;
	}


	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override
	{
		return TRUE;
	}


	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override
	{
		m_Current += nCurent;
		return m_Callback->ErasureProgress(m_Step, m_Max, m_Current);
	}

private:
};

DWORD CErasure::UnuseSpaceErasure(TCHAR* Driver, CErasureMethod& method, IErasureCallback* callback)
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
		WORD wCompressStatus;
		
		m_tmpDir = Driver;
		m_tmpDir += +_T("\\");
		Result = CFileUtils::GetCompressStatus(m_tmpDir, &wCompressStatus);
		BOOL IsCompressed = wCompressStatus & COMPRESSION_FORMAT_NONE;
		if (IsCompressed)
			CFileUtils::SetCompression(Driver, FALSE);
		m_tmpDir += Erasure_temp_path;
		Result = CFileUtils::ForceDirectories(m_tmpDir);
		if (Result != 0)
			break;
		m_tmpDir += _T("\\");
		Result = EraseUnusedSpace();
		
		if (Result == 0)
		{
			Result = EraseMftFileRecord();
// 			if (Result != 0 && Result != ERROR_DISK_FULL) //不管有没有错误删除临时文件
// 				break;
		}

		Result = DeleteTempFiles();
		if(Result != 0)
			break;
		RemoveDirectory(m_tmpDir);
	} while (false);
	

	return Result;
}

DWORD CErasure::EraseUnusedSpace()
{
	DWORD Result = 0;
	UINT64 nFreeSpace;
	UINT64 nFileSize;


	Result = m_volInfo.GetAvailableFreeSpace(&nFreeSpace);
	if (Result != 0)
		return Result;

	if (!m_callback->ErasureProgress(1, nFreeSpace * m_method->GetPassCount(), 0))
		return ERROR_CANCELED;

	IErasureCallbackImpl callback;
	callback.m_Step = 1;
	callback.m_Max = nFreeSpace * m_method->GetPassCount();
	callback.m_Callback = m_callback;

	while (true)
	{
		Result = m_volInfo.GetAvailableFreeSpace(&nFreeSpace);
		if(Result != 0)
			break;
		nFileSize = min(MAX_TEMPFILESIZE, nFreeSpace);
		if(nFileSize == 0)
			break;
		Result = EraseFreeSpace(nFileSize, &callback);
		if (Result != 0)
			break;
	}
	
	if (!m_callback->ErasureCompleted(1, Result))
		return ERROR_CANCELED;

	return Result;
}

DWORD CErasure::EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	DWORD Result;

	if (!callbck->ErasureStart(m_method->GetPassCount()))
		return ERROR_CANCELED;

	IErasureCallbackImpl callimpl;
	callimpl.m_Callback = callbck;
	callimpl.m_Max = nFileSize * m_method->GetPassCount();


	for (int i = 0; i < m_method->GetPassCount(); i++)
	{
		callimpl.m_Step = i + 1;

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
		if(Result != 0)
			break;
		if (!callbck->ErasureCompleted(i, Result))
			return ERROR_CANCELED;
	}

	return Result;
}

UINT64 xor128(void)
{
	static UINT64 x = 0x123456789ABCDEF0;
	static UINT64 y = 0x2436069DF85A43C9;
	static UINT64 z = 0x5ab2128ed862cf39;
	static UINT64 w = 0xbc886aaa751eef23;
	UINT64 t;
	t = x ^ (x << 22);

	x = y; y = z; z = w;
	return w = w ^ (w >> 49) ^ (t ^ (t >> 18));
}

DWORD CErasure::WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	if(SetFilePointer(hFile, nStartPos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();

#define MAX_BUFFER_SIZE 1024

	BYTE Buffer[MAX_BUFFER_SIZE];
	DWORD dwCb;
	UINT64 nCurrent = 0;
	while (nFileSize > nCurrent)
	{
		UINT nBufferSize = min(MAX_BUFFER_SIZE, nFileSize - nCurrent);
		for (int i = 0; i < nBufferSize / sizeof(UINT64); i++)
			((PUINT64)Buffer)[i] = xor128();
		
		if (!WriteFile(hFile, Buffer, nBufferSize, &dwCb, NULL))
		{
			return GetLastError();
		}
		nCurrent += nBufferSize;

		if (!callbck->ErasureProgress(1, nFileSize, nCurrent))
			return ERROR_CANCELED;
	}
	return 0;
}

DWORD CErasure::WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount)
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

		if (!callbck->ErasureProgress(1, nFileSize, nCurrent))
			return ERROR_CANCELED;
	}
	delete[] Buffer;

	return Result;
}

DWORD CErasure::EraseMftFileRecord()
{
	VOLUME_FILE_SYSTEM fs;
	DWORD Result = m_volInfo.GetFileSystem(&fs);
	switch (fs)
	{
	case FS_NTFS:
		Result = EraseNtfsDeletedFile();
		break;
	default:
		Result = EraseMftDeleteFile();
		break;
	}

	m_callback->ErasureCompleted(2, Result);
	return Result;
}

DWORD CErasure::EraseNtfsDeletedFile()
{
	DWORD Result = 0;
	DWORD dwBytesPreFile;
	UINT64 nTotalSize;
	do 
	{
		Result = CNtfsUtils::GetBytesPerFileRecord(m_volInfo, &dwBytesPreFile);
		if (Result != 0)
			break;
		Result = CNtfsUtils::GetMftValidSize(m_volInfo, &nTotalSize);
		if(Result != 0)
			break;
		m_callback->ErasureProgress(2, nTotalSize, 0);

		IErasureCallbackImpl callback;
		callback.m_Step = 2;
		callback.m_Max = nTotalSize;
		callback.m_Callback = m_callback;
		while (true)
		{
			Result = EraseFreeSpace(dwBytesPreFile, &callback);
			if (Result != 0)
			{
				if (dwBytesPreFile == 0)
					break;
				dwBytesPreFile--;
			}
		}
	} while (false);

	return Result;
}

DWORD CErasure::CreateTempFile(UINT64 nFileSize, HANDLE* pOut)
{
	DWORD Result = 0;
	//TCHAR tmpName[MAX_PATH] = { 0 };
	CLdString tmpName((UINT)MAX_PATH);
	GenerateRandomFileName(20, tmpName);
	//if (!GetTempFileName(m_tmpDir, _T("__LD"), 0, tmpName)) //磁盘没有空间时这个函数失败
		//GetLastError();
	tmpName = m_tmpDir + tmpName;
	HANDLE hFile = CreateFile(tmpName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	
	*pOut = hFile;
	FILE_END_OF_FILE_INFO einfo = { 0 };
	einfo.EndOfFile.QuadPart = nFileSize;
	if (!SetFileInformationByHandle(hFile, FileEndOfFileInfo, &einfo, sizeof(FILE_END_OF_FILE_INFO)))
	{
		Result = GetLastError();
	}
	m_Tmpfiles.Add(tmpName);
	return Result;
}

DWORD CErasure::ResetFileDate(HANDLE hFile)
{
	FILE_BASIC_INFO binfo = { 0 };
	SYSTEMTIME ts = { 0 };

	ts.wYear = 1601;
	ts.wMonth = 1;
	ts.wDay = 1;
	ts.wMinute = 1;
	FILETIME ft;
	SystemTimeToFileTime(&ts, &ft);

	binfo.ChangeTime.HighPart = ft.dwHighDateTime;
	binfo.ChangeTime.LowPart = ft.dwLowDateTime;

	binfo.LastAccessTime.HighPart = ft.dwHighDateTime;
	binfo.LastAccessTime.LowPart = ft.dwLowDateTime;

	binfo.LastWriteTime.HighPart = ft.dwHighDateTime;
	binfo.LastWriteTime.LowPart = ft.dwLowDateTime;

	binfo.CreationTime.HighPart = ft.dwHighDateTime;
	binfo.CreationTime.LowPart = ft.dwLowDateTime;

	binfo.FileAttributes = FILE_ATTRIBUTE_SYSTEM;

	//return NtSetInformationFile(hFile, &binfo, sizeof(FILE_BASIC_INFO), FileBasicInformation);
	if (!SetFileInformationByHandle(hFile, FileBasicInfo, &binfo, sizeof(FILE_BASIC_INFO)))
		return GetLastError();
	return 0;
}

DWORD CErasure::EraseFreeSpace(UINT64 nFileSize, IErasureCallback* callback)
{
	DWORD Result = 0;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	Result = CreateTempFile(nFileSize, &hFile);
	if (Result != 0 && hFile == INVALID_HANDLE_VALUE)
		return Result;  
	//文件创建成功，但设置文件长度失败
	Result = EraseFile(hFile, 0, nFileSize, callback);
	ResetFileDate(hFile);
	CloseHandle(hFile);
	return Result;
}

DWORD CErasure::DeleteTempFiles()
{
	DWORD Result = 0;
	m_callback->ErasureProgress(3, m_Tmpfiles.GetCount(), 0);
	for (int i = 0; i < m_Tmpfiles.GetCount(); i++)
	{
		HANDLE hFile = CreateFile(m_Tmpfiles[i], GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			continue;
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		DeleteFile(m_Tmpfiles[i]);
		m_callback->ErasureProgress(3, m_Tmpfiles.GetCount(), i);
		//CFileUtils::RenameFileName(m_Tmpfiles[i]);
	}
	m_callback->ErasureCompleted(3, Result);
	return Result;
}

VOID CErasure::GenerateRandomFileName(int length, CLdString& Out)
{
	TCHAR validFileNameChars[] = _T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ _+=-()[]{}',`~!");
	for (int j = 0; j < length; j++)
	{
		Out.SetAt(j, validFileNameChars[(rand() % ARRAYSIZE(validFileNameChars)) - 1]);
	}
}

DWORD CErasure::EraseMftDeleteFile()
{
	DWORD Result = 0;
	UINT64 nTotalSize = 10000; //todo
	do
	{
		m_callback->ErasureProgress(2, nTotalSize, 0);

		IErasureCallbackImpl callback;
		callback.m_Step = 2;
		callback.m_Max = nTotalSize;
		callback.m_Callback = m_callback;
		while (true)
		{
			Result = EraseFreeSpace(0, &callback);
			if (Result != 0)
			{
				break;
			}
		}
	} while (false);

	return Result;
}

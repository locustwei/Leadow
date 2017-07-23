#include "stdafx.h"
#include "Erasure.h"
#include "Winerror.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp")
#define MIN_TEMPFILESIZE 1024 * 1024 * 256   //历时文件最小值（文件太小会影响擦除速度）
#define MAX_TEMPFILESIZE 1024 * 1024 * 4096

CErasure::CErasure():
	m_Tmpfiles(),
	m_tmpDir(),
	m_volInfo()
{
	m_method = NULL;
}


CErasure::~CErasure()
{
}
/*!
分步进行回掉函数实现
 */
class CErasureCallbackImpl : public IErasureCallback
{
public:
	IErasureCallback* m_Callback;
	UINT64 m_Max, m_Current, m_Step;

	CErasureCallbackImpl()
	{
		m_Current = 0;
		m_Max = 0;
		m_Step = 0;
		m_Callback = NULL;
	};

	~CErasureCallbackImpl()
	{
	};


	virtual BOOL ErasureStart(UINT nStepCount) override
	{
		return m_Callback->ErasureStart(nStepCount);
	}


	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override
	{
		return m_Callback->ErasureCompleted(nStep, dwErroCode);
	}


	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override
	{
		m_Current += nCurent;
		return m_Callback->ErasureProgress(m_Step, m_Max, m_Current);
	}

private:
};

DWORD CErasure::UnuseSpaceErasure(TCHAR* Driver, CErasureMethod* method, IErasureCallback* callback)
{
	//检查是否有管理员权限（错误）
	//检查是否有系统还原点（警告）
	//检查磁盘用户配额（警告 GetDiskFreeSpaceEx）
	DWORD Result;

	do
	{
		if(!callback->ErasureStart(3))
			return ERROR_CANCELED;

		m_method = method;

		Result = m_volInfo.OpenVolumePath(Driver);
		if (Result != 0)
			break;
		
		m_tmpDir = Driver;
		m_tmpDir += '\\';

		WORD wCompressStatus;
		Result = CFileUtils::GetCompressStatus(m_tmpDir, &wCompressStatus);
		if(Result == 0)
		{
			if (wCompressStatus & COMPRESSION_FORMAT_NONE)
				CFileUtils::SetCompression(Driver, FALSE);
		}
		m_tmpDir += Erasure_temp_path;
		Result = CFileUtils::ForceDirectories(m_tmpDir + _T("\\"));
		if (Result != 0)
			break;
		m_tmpDir += _T("\\");

		Result = UnusedSpace2TempFile(callback);
		
		if (Result == 0)
			EraseMftFileRecord(callback);

		Result = DeleteTempFiles(callback);
		RemoveDirectory(m_tmpDir);

	} while (false);
	
	callback->ErasureCompleted(3, Result);

	return Result;
}

DWORD CErasure::FileErasure(TCHAR * lpFileName, CErasureMethod * method, IErasureCallback * callbck)
{
	DWORD result = 0;

	m_method = method;
	
	if (!callbck->ErasureStart(method->GetPassCount()))
		return ERROR_CANCELED;
//
//	DWORD dwAttr = GetFileAttributes(lpFileName);
//	if((dwAttr & FILE_ATTRIBUTE_DIRECTORY)==0)
//	{
//		HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
//		if (hFile == INVALID_HANDLE_VALUE)
//			return GetLastError();
//
//		LARGE_INTEGER fileSize;
//		if (!GetFileSizeEx(hFile, &fileSize))
//			result = GetLastError();
//
//
//		if (result == 0 && fileSize.QuadPart > 0)
//			result = EraseFile(hFile, 0, fileSize.QuadPart, callbck);
//
//		CloseHandle(hFile);
//	}
//
//	m_Tmpfiles.Add(lpFileName);
//	if(result == 0)
//		result = DeleteTempFiles(callbck);

	callbck->ErasureCompleted(method->GetPassCount(), result);

	//DebugOutput(L"%d %s\n", result, lpFileName);

	return result;
}

DWORD CErasure::UnusedSpace2TempFile(IErasureCallback* callback)
{
	DWORD Result;
	UINT64 nFreeSpace;
	UINT nFileSize;
	UINT nFileCount; //被删除的文件数。（估算）

	Result = m_volInfo.GetTotalSize(&nFreeSpace);
	if (Result == 0)
		return Result;
	Result = m_volInfo.GetClusterSize(&nFileSize);
	if (Result == 0)
		return Result;

	nFileCount = nFreeSpace / 10 / nFileSize;

	Result = m_volInfo.GetAvailableFreeSpace(&nFreeSpace);
	if (Result != 0)
		return Result;
	nFileSize = nFreeSpace / nFileCount;
	if (nFileSize > MAX_TEMPFILESIZE)
		nFileSize = MAX_TEMPFILESIZE;
	if (nFileSize < MIN_TEMPFILESIZE)
		nFileSize = MIN_TEMPFILESIZE;
	nFileCount = nFreeSpace / nFileCount;

	if (callback && !callback->ErasureProgress(1, nFileCount + 1, 0))
		return ERROR_CANCELED;

	int tryCount = 3;
	int nCount = 0;
	while (true)
	{
		Result = m_volInfo.GetAvailableFreeSpace(&nFreeSpace);
		if(Result != 0 || nFreeSpace==0)
			break;
		HANDLE hFile = INVALID_HANDLE_VALUE;
		Result = CreateTempFile(nFileSize > nFreeSpace? nFreeSpace:nFileSize, &hFile);
		if (Result != 0)
		{
			tryCount--; //重试3次
			if (tryCount == 0)
				break;
			else
				continue;
		}

		tryCount = 3;
		Result = EraseFile(hFile, 0, nFileSize, NULL);
		if (Result != 0)
			break;

		if (callback && !callback->ErasureProgress(1, nFileCount + 1, nCount++))
			return ERROR_CANCELED;
	}
	
	return Result;
}

DWORD CErasure::EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	DWORD Result = 0;

	for (int i = 0; i < m_method->GetPassCount(); i++)
	{
		ErasureMethodPass* pd = m_method->GetPassData(i);
		switch (pd->function)
		{
		case WriteRandom:
			Result = WriteFileRandom(hFile, nStartPos, nFileSize, callbck);
			break;
		case WriteConstant:
			Result = WriteFileConst(hFile, nStartPos, nFileSize, callbck, pd->bytes, pd->nCount);
			break;
		}
		if(Result != 0)
			break;
	}
	return Result;
}

UINT64 Rand64(void)
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

#define MAX_BUFFER_SIZE 1024 * 1024

DWORD CErasure::WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	if (callbck && !callbck->ErasureProgress(1, nFileSize - nStartPos, 0))
		return ERROR_CANCELED;

	if(SetFilePointer(hFile, nStartPos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();

	DWORD result = 0, dwCb, nBufferSize = min(nFileSize - nStartPos, MAX_BUFFER_SIZE);

	PBYTE Buffer = new BYTE[nBufferSize];
	for (int i = 0; i < nBufferSize / sizeof(UINT64); i++)
		((PUINT64)Buffer)[i] = Rand64();

	UINT64 nCurrent = 0;
	int tryCount = 3;
	while (nFileSize > nCurrent)
	{
		UINT nSize = min(nBufferSize, nFileSize - nStartPos - nCurrent);
		
		if (!WriteFile(hFile, Buffer, nSize, &dwCb, NULL))
		{
			tryCount--;
			if (tryCount == 0)
			{
				result = GetLastError();
				break;
			}
			else
				continue;
		}
		nCurrent += nSize;

		if (callbck && !callbck->ErasureProgress(1, nFileSize-nStartPos, nCurrent))
			return ERROR_CANCELED;
	}
	delete [] Buffer;

	return result;
}

DWORD CErasure::WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount)
{
	if (callbck && !callbck->ErasureProgress(1, nFileSize - nStartPos, 0))
		return ERROR_CANCELED;

	if(SetFilePointer(hFile, nStartPos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();
	
	DWORD result = 0, dwCb, nBufferSize = min(nFileSize - nStartPos, MAX_BUFFER_SIZE);
	nBufferSize = nBufferSize - (nBufferSize % nCount);
	PBYTE Buffer = new BYTE[nBufferSize];
	for (int i = 0; i < nBufferSize / nCount; i++)
		memcpy(Buffer + i*nCount, bytes, nCount);

	UINT64 nCurrent = 0;
	int tryCount = 3;
	while (nFileSize > nCurrent)
	{
		UINT nSize = min(nBufferSize, nFileSize - nStartPos - nCurrent);
		if (!WriteFile(hFile, Buffer, nSize, &dwCb, NULL))
		{
			tryCount--;
			if (tryCount == 0)
			{
				result = GetLastError();
				break;
			}
			else
				continue;
			
		}
		nCurrent += nSize;

		if (callbck && !callbck->ErasureProgress(1, nFileSize -nStartPos, nCurrent))
			return ERROR_CANCELED;
	}
	delete[] Buffer;

	return result;
}

DWORD CErasure::EraseMftFileRecord(IErasureCallback* callback)
{
	VOLUME_FILE_SYSTEM fs;
	DWORD Result = m_volInfo.GetFileSystem(&fs);
	if (Result != 0)
		return Result;

	switch (fs)
	{
	case FS_NTFS:
		Result = EraseNtfsDeletedFile(callback);
		break;
	default:
		Result = EraseMftDeleteFile(callback);
		break;
	}

	callback->ErasureCompleted(2, Result);
	return Result;
}

DWORD CErasure::EraseNtfsDeletedFile(IErasureCallback* callback)
{
	DWORD result;
	DWORD dwBytesPreFile;
	UINT64 nTotalSize;

	do 
	{
		result = CNtfsUtils::GetBytesPerFileRecord(m_volInfo, &dwBytesPreFile);
		if (result != 0)
			break;
		result = CNtfsUtils::GetMftValidSize(m_volInfo, &nTotalSize);
		if(result != 0)
			break;
		if (callback && !callback->ErasureProgress(1, nTotalSize, 0))
			return ERROR_CANCELED;

		int nCount = 0;
		while (true)
		{
			result = EraseFreeSpace(dwBytesPreFile, NULL);
			if (result != 0)
			{
				if (dwBytesPreFile == 0)
					break;
				dwBytesPreFile--;
			}

			if (callback && !callback->ErasureProgress(1, nTotalSize, nCount++))
				return ERROR_CANCELED;
		}
	} while (false);

	return result;
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
	DWORD Result;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	Result = CreateTempFile(nFileSize, &hFile);
	if (Result != 0 && hFile == INVALID_HANDLE_VALUE)
		return Result;  
	//文件创建成功，但设置文件长度失败
	Result = EraseFile(hFile, 0, nFileSize, callback);

	CloseHandle(hFile);
	return Result;
}

DWORD CErasure::DeleteTempFiles(IErasureCallback* callback)
{
	DWORD Result = 0;
	if (callback)
		callback->ErasureProgress(3, m_Tmpfiles.GetCount(), 0);
	
	for (int i = 0; i < m_Tmpfiles.GetCount(); i++)
	{
		HANDLE hFile = CreateFile(m_Tmpfiles[i], GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			SetEndOfFile(hFile);
			ResetFileDate(hFile);
			CloseHandle(hFile);
		};
		
		Result = CFileUtils::ShDeleteFile(m_Tmpfiles[i]);
		
		if(callback)
			callback->ErasureProgress(3, m_Tmpfiles.GetCount(), i);
	}
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

DWORD CErasure::EraseMftDeleteFile(IErasureCallback* callback)
{
	DWORD Result = 0;
	UINT64 nTotalSize = 10000; //todo
	DWORD nFileSize = 1024; //todo
	do
	{
		if (!callback->ErasureProgress(2, nTotalSize, 0))
			break;

		int nCount = 0;
		while (true)
		{
			Result = EraseFreeSpace(nFileSize, NULL);
			if (Result != 0)
			{
				if(nFileSize == 0)
					break;
				nFileSize--;
			}
			if (!callback->ErasureProgress(2, nTotalSize, nCount++))
				break;
		}
	} while (false);

	return Result;
}

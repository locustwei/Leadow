#include "stdafx.h"
#include "Erasure.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp")
#define MIN_TEMPFILESIZE 1024 * 1024 * 256   //历时文件最小值（文件太小会影响擦除速度）
#define MAX_TEMPFILESIZE 0x100000000 //(INT64)1024 * (INT64)1024 * (INT64)4096

CErasure::CErasure():
	m_Tmpfiles(),
	m_tmpDir(),
	m_volInfo(),
	m_DeleteFileTraces(0)
{
	m_method = NULL;
}


CErasure::~CErasure()
{
}

DWORD CErasure::UnuseSpaceErasure(CVolumeInfo* pvolume, CErasureMethod* method, IErasureCallback* callback)
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
		m_volInfo = pvolume;

		m_tmpDir = m_volInfo->GetFullName();

		WORD wCompressStatus;
		Result = CFileUtils::GetCompressStatus(m_tmpDir, &wCompressStatus);
		if(Result == 0)
		{
			if (wCompressStatus & COMPRESSION_FORMAT_NONE)
				CFileUtils::SetCompression(m_volInfo->GetFullName(), FALSE);
		}
		m_tmpDir += Erasure_temp_path;
		Result = CFileUtils::ForceDirectories(m_tmpDir + _T("\\"));
		if (Result != 0)
			break;
		m_tmpDir += _T("\\");

		Result = EraseFreeDataSpace(callback);
		
		if (Result == 0)
			Result = EraseFreeMftSpace(callback);
		
		DeleteTempFiles(callback);

		if (Result == 0)
			Result = EraseDelFileTrace(callback);

	} while (false);
	
	DeleteTempFiles(callback);
	RemoveDirectory(m_tmpDir);

	callback->ErasureCompleted(3, Result);

	return Result;
}

DWORD CErasure::FileErasure(TCHAR * lpFileName, CErasureMethod * method, IErasureCallback * callbck)
{
	DWORD result = 0;

	m_method = method;
	
	if (!callbck->ErasureStart(method->GetPassCount()))
		return ERROR_CANCELED;

	DWORD dwAttr = GetFileAttributes(lpFileName);
	if((dwAttr & FILE_ATTRIBUTE_DIRECTORY)==0)
	{
		HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return GetLastError();

		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(hFile, &fileSize))
			result = GetLastError();


		if (result == 0 && fileSize.QuadPart > 0)
			result = EraseFile(hFile, 0, fileSize.QuadPart, callbck);

		CloseHandle(hFile);

		m_Tmpfiles.Add(lpFileName);
		if (result == 0)
			result = DeleteTempFiles(callbck);
	}else
	{
		if (!RemoveDirectory(lpFileName))
			result = GetLastError();
	}

	callbck->ErasureCompleted(method->GetPassCount(), result);

	//DebugOutput(L"%d %s\n", result, lpFileName);

	return result;
}

DWORD CErasure::AnalysisVolume(CVolumeInfo* pvolume)
{
	CMftReader* reader = CMftReader::CreateReader(pvolume);
	if (!reader)
		return -1;
	reader->SetHolder(this);
	reader->EnumFiles(1);
	PVOLUME_BPB_DATA pData = pvolume->GetVolumeMftData();
	pvolume->GetAvailableFreeSpace();
	pvolume->GetTotalSize();
	
	return 0;
}

DWORD CErasure::EraseFreeDataSpace(IErasureCallback* callback)
{
	DWORD result;
	UINT64 nFreeSpace;
	UINT64 nFileSize;
	UINT64 nFileCount; //被删除的文件数。（估算）

	nFreeSpace = m_volInfo->GetAvailableFreeSpace(&result);
	if (result != 0)
		return result;

	if (nFreeSpace > MAX_TEMPFILESIZE)
		nFileSize = MAX_TEMPFILESIZE;
	else
		nFileSize = nFreeSpace;
	nFileCount = nFreeSpace / nFileSize;

	int nCount = 0;
	if (callback && !callback->ErasureProgress(1, nFileCount + 1, nCount++))
		return ERROR_CANCELED;

	while (true)
	{
		nFreeSpace = m_volInfo->GetAvailableFreeSpace();
		if(nFreeSpace==0)
			break;
		nFileSize = nFileSize > nFreeSpace ? nFreeSpace : nFileSize;
		
		result = CrateTempFileAndErase(nFileSize, callback);
		if (result != 0)
			break;
		if (callback && !callback->ErasureProgress(1, nFileCount + 1, nCount++))
			return ERROR_CANCELED;
	}
	
	return result;
}

DWORD CErasure::EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	DWORD Result = 0;

	for (UINT i = 0; i < m_method->GetPassCount(); i++)
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
	LARGE_INTEGER pos; pos.QuadPart = nStartPos;
	if(SetFilePointerEx(hFile, pos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();

	DWORD result = 0, dwCb, nBufferSize = (DWORD)min(nFileSize - nStartPos, MAX_BUFFER_SIZE);

	PBYTE Buffer = new BYTE[nBufferSize];
	for (int i = 0; i < nBufferSize / sizeof(UINT64); i++)
		((PUINT64)Buffer)[i] = Rand64();

	UINT64 nCurrent = 0;
	int tryCount = 3;
	while (nFileSize > nCurrent)
	{
		UINT nSize = (UINT)min(nBufferSize, nFileSize - nStartPos - nCurrent);
		
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
	LARGE_INTEGER pos; pos.QuadPart = nStartPos;
	if(SetFilePointerEx(hFile, pos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();
	
	DWORD result = 0, dwCb, nBufferSize = (DWORD)min(nFileSize - nStartPos, MAX_BUFFER_SIZE);
	nBufferSize = nBufferSize - (nBufferSize % nCount);
	PBYTE Buffer = new BYTE[nBufferSize];
	for (UINT i = 0; i < nBufferSize / nCount; i++)
		memcpy(Buffer + i*nCount, bytes, nCount);

	UINT64 nCurrent = 0;
	int tryCount = 3;
	while (nFileSize > nCurrent)
	{
		UINT nSize = (DWORD)min(nBufferSize, nFileSize - nStartPos - nCurrent);
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

DWORD CErasure::EraseFreeMftSpace(IErasureCallback* callback)
{
	DWORD result;
	VOLUME_FILE_SYSTEM fs = m_volInfo->GetFileSystem(&result);
	if (result != 0)
		return result;

	switch (fs)
	{
	case FS_NTFS:
		result = EraseNtfsFreeSpace(callback);
		break;
	case FS_FAT32:
		result = EraseFatFreeSpace(callback);
		break;
	case FS_UNKNOW: break;
	case FS_FAT16: break;
	case FS_FAT12: break;
	case FS_EXFAT: break;
	default: break;
	}

	return result;
}

DWORD CErasure::EraseNtfsFreeSpace(IErasureCallback* callback)
{
	DWORD result;
	DWORD dwBytesPreFile;
	UINT64 nMaxFileCount;
	PVOLUME_BPB_DATA pBpb = m_volInfo->GetVolumeMftData(&result);
	do 
	{
		if (result != 0)
			break;

		dwBytesPreFile = pBpb->BytesPerFileRecordSegment;
		nMaxFileCount = pBpb->MftValidDataLength / dwBytesPreFile;

		if (callback && !callback->ErasureProgress(1, nMaxFileCount, 0))
			return ERROR_CANCELED;

		int nCount = 0;
		while (true)
		{
			result = CrateTempFileAndErase(dwBytesPreFile, nullptr);
			if (result != 0)
			{
				if (dwBytesPreFile == 0)
					break;
				dwBytesPreFile--;
			}

			if (callback && !callback->ErasureProgress(1, nMaxFileCount, nCount++))
				return ERROR_CANCELED;
		}
	} while (false);

	if (result == ERROR_DISK_FULL)
		result = 0;
	return result;
}

DWORD CErasure::EraseFatFreeSpace(IErasureCallback* callback)
{//这里没事可干
	return 0;
}

DWORD CErasure::CreateTempFile(UINT64 nFileSize, HANDLE* pOut, int nFileNameLength)
{
	DWORD Result = 0;
	CLdString tmpName;
	CFileUtils::GenerateRandomFileName(nFileNameLength, tmpName);

	tmpName = m_tmpDir + tmpName;
	HANDLE hFile = CreateFile(tmpName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	if(pOut)
		*pOut = hFile;
	if (nFileSize != 0)
	{
		FILE_END_OF_FILE_INFO einfo = { 0 };
		einfo.EndOfFile.QuadPart = nFileSize;
		if (!SetFileInformationByHandle(hFile, FileEndOfFileInfo, &einfo, sizeof(FILE_END_OF_FILE_INFO)))
		{
			if (SetFilePointerEx(hFile, einfo.EndOfFile, nullptr, 0))
				SetEndOfFile(hFile);
			//Result = GetLastError();
		}
	}
	if (!pOut)
		CloseHandle(hFile);

	m_Tmpfiles.Add(tmpName);
	return Result;
}

DWORD CErasure::ResetFileDate(HANDLE hFile)
{
	FILE_BASIC_INFO binfo = { 0 };
	SYSTEMTIME ts = { 0 };

	ts.wYear = 1980;
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

	binfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;

	//return NtSetInformationFile(hFile, &binfo, sizeof(FILE_BASIC_INFO), FileBasicInformation);
	if (!SetFileInformationByHandle(hFile, FileBasicInfo, &binfo, sizeof(FILE_BASIC_INFO)))
		return GetLastError();
	return 0;
}

DWORD CErasure::CrateTempFileAndErase(UINT64 nFileSize, IErasureCallback* callback)
{
	DWORD Result;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	Result = CreateTempFile(nFileSize, &hFile);
	if (Result != 0 && hFile == INVALID_HANDLE_VALUE)
		return Result;  
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
		}else
		{
			Result = GetLastError();
		}
		
		if (!DeleteFile(m_Tmpfiles[i]))
			Result = GetLastError();
//		Result = CFileUtils::ShDeleteFile(m_Tmpfiles[i]);
		
		if(callback)
			callback->ErasureProgress(3, m_Tmpfiles.GetCount(), i);
	}
	return Result;
}

BOOL CErasure::EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param)
{
	if(Param == 1)
	{
		if (!pFileInfo)
		{
			if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DELETED)
			{
				m_DeleteFileTraces++;
			}
		}
	}
	return Param == 0;
}

DWORD CErasure::EraseNtfsTrace(IErasureCallback* callback)
{
	DWORD result;
	UINT64 nMftSize;
	PVOLUME_BPB_DATA pBpb = m_volInfo->GetVolumeMftData(&result);
	do
	{
		if (result != 0)
			break;
		nMftSize = pBpb->MftValidDataLength;
		if (result != 0)
			break;
		int pollingInterval = (int)min(max(1, nMftSize / pBpb->BytesPerClusters / 20), 128);
		int totalFiles = (int)max(1L, nMftSize / pBpb->BytesPerFileRecordSegment);

		if (callback && !callback->ErasureProgress(1, totalFiles, 0))
			return ERROR_CANCELED;

		int nCount = 0, ntry = 3;
		while (true)
		{
			result = CreateTempFile(0, NULL, 220);
			if (result != 0)
			{
				ntry--;
				if (ntry < 0)
					break;
				continue;
			}
			ntry = 3;
			nCount++;
			if (nCount % pollingInterval == 0)
			{
				m_volInfo->RefreshBpbData();
				if (pBpb->MftValidDataLength > nMftSize)
					break;
			}
			if (callback && !callback->ErasureProgress(1, totalFiles, nCount))
				return ERROR_CANCELED;
		}
	} while (false);

	return result;

}

DWORD CErasure::EraseFatTrace(IErasureCallback* callback)
{
	CMftReader* reader = CMftReader::CreateReader(m_volInfo);
	reader->SetHolder(this);
	//reader->EnumFiles(nullptr);
	DWORD dwFileCount = 0;
	DWORD result = ((CFatMftReader*)reader)->EraseTrace(dwFileCount);
	delete reader;

	return 0;
}

DWORD CErasure::EraseDelFileTrace(IErasureCallback* callback)
{
	DWORD result;
	VOLUME_FILE_SYSTEM fs = m_volInfo->GetFileSystem(&result);
	if (result != 0)
		return result;

	switch (fs)
	{
	case FS_NTFS:
		result = EraseNtfsTrace(callback);
		break;
	case FS_FAT32:
		result = EraseFatTrace(callback);
		break;
	case FS_UNKNOW: break;
	case FS_FAT16: break;
	case FS_FAT12: break;
	case FS_EXFAT: break;
	default: break;
	}

	return result;

}

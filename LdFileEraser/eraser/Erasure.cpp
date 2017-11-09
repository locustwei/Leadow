#include "stdafx.h"
#include "Erasure.h"
#include "../../MftLib/FatMftReader.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp") //存放临时文件的临时文件夹
#define MIN_TEMPFILESIZE 1024 * 1024 * 256   //历时文件最小值（文件太小会影响擦除速度）
#define MAX_TEMPFILESIZE 0x100000000 //(INT64)1024 * (INT64)1024 * (INT64)4096
#define MAX_BUFFER_SIZE 1024 * 1024

CErasure::CErasure()
	:m_Volume()
	,m_tmpDir()
	,m_Tmpfiles()
{
	m_method = NULL;
}


CErasure::~CErasure()
{
}

DWORD CErasure::UnuseSpaceErasure(CVolumeEx* pvolume, CErasureMothed* method, IErasureCallback* callback, BOOL bSkipSpace, BOOL bSkipTrack)
{
	//检查是否有管理员权限（错误）
	//检查是否有系统还原点（警告）
	//检查磁盘用户配额（警告 GetDiskFreeSpaceEx）
	DWORD result;

	do
	{
		if(!callback->ErasureStart())
			return ERROR_CANCELED;

		m_method = method;
		m_Volume = pvolume;

		WORD wCompressStatus;
		result = CFileUtils::GetCompressStatus(m_tmpDir, &wCompressStatus);
		if(result == 0)
		{
			if (wCompressStatus & COMPRESSION_FORMAT_NONE)
				CFileUtils::SetCompression(m_Volume->GetFullName(), FALSE);
		}

		result = CreateTempDirectory();
		if (result != 0)
			break;

		result = EraseFreeDataSpace(callback);
				

		if (result == 0)
			result = EraseDelFileTrace(callback);

	} while (false);
	
	DeleteTempFiles(callback);
	RemoveDirectory(m_tmpDir);

	callback->ErasureCompleted(result);

	return result;
}

DWORD CErasure::FileErasure(TCHAR * lpFileName, CErasureMothed * method, IErasureCallback * callbck)
{
	DWORD result = 0;

	m_method = method;
	
	if (!callbck->ErasureStart())
		return ERROR_CANCELED;

	DWORD dwAttr = GetFileAttributes(lpFileName);
	SetFileAttributes(lpFileName, FILE_ATTRIBUTE_NORMAL);

	HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		result = GetLastError();
		if (result == ERROR_ACCESS_DENIED) //重试一次
			ClearFileSecurity(lpFileName);
		hFile = CreateFile(lpFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			result = GetLastError();
	}
		
	LARGE_INTEGER fileSize;
	if (result == 0)
	{
		if (!GetFileSizeEx(hFile, &fileSize))
			result = GetLastError();
	}

	if (result == 0 && fileSize.QuadPart > 0)
		result = EraseFile(hFile, 0, fileSize.QuadPart, callbck);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
	}

	//擦除交换数据流
	CLdArray<PFILE_ADS_INFO> fileStreamNames;
	if (CFileUtils::GetFileADSNames(lpFileName, &fileStreamNames) == 0)
	{
		for(int i=0; i<fileStreamNames.GetCount(); i++)
		{
			CLdString fileName = lpFileName;
			fileName += fileStreamNames[i]->StreamName;
			HANDLE hStream = CreateFile(fileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hStream != INVALID_HANDLE_VALUE)
			{
				LARGE_INTEGER StreamSize;
				if (result == 0)
				{
					if (!GetFileSizeEx(hStream, &StreamSize))
						result = GetLastError();
				}

				if (result == 0 && StreamSize.QuadPart > 0)
					result = EraseFile(hStream, 0, StreamSize.QuadPart, callbck);
				CloseHandle(hStream);
			}
			delete fileStreamNames[i];
		}
	}

	if (result == 0)
	{  //删除文件，先重命名成一个随机文件名
		CLdString tmpName;
		CFileUtils::GenerateRandomFileName(120, &tmpName);
		result = CFileUtils::RenameFile(lpFileName, tmpName);
		CLdString* path;
		if(result == 0)
		{
			path = new CLdString((UINT)MAX_PATH);
			CFileUtils::ExtractFilePath(lpFileName, path->GetData());
			if (!path->IsEmpty() && path->GetData()[path->GetLength() - 1] != '\\')
				*path += '\\';
			*path += tmpName.GetData();

		}else
		{//重命名失败，就睁一只眼闭一只眼吧。
			path = new CLdString(lpFileName);
			result = 0;
		}

		m_Tmpfiles.Add(path/*new CLdString(lpFileName)*/);
		result = DeleteTempFiles(callbck);

	}

	callbck->ErasureCompleted(result);

	//DebugOutput(L"%d %s\n", result, lpFileName);

	return result;
}

DWORD CErasure::DirectoryErasure(TCHAR * lpDirName, IErasureCallback * callbck)
{
	DWORD result = 0;

	if (!callbck->ErasureStart())
		return ERROR_CANCELED;

	CLdString tmpName;
	CFileUtils::GenerateRandomFileName(120, &tmpName);
	result = CFileUtils::RenameFile(lpDirName, tmpName);
	CLdString* path;
	if (result == 0)
	{
		path = new CLdString((UINT)MAX_PATH);
		CFileUtils::ExtractFilePath(lpDirName, path->GetData());
		if (!path->IsEmpty() && path->GetData()[path->GetLength() - 1] != '\\')
			*path += '\\';
		*path += tmpName.GetData();

	}
	else
	{//重命名失败，就睁一只眼闭一只眼吧。
		path = new CLdString(lpDirName);
		result = 0;
	}

	if (!RemoveDirectory(path->GetData()))
		result = GetLastError();
	delete path;

	callbck->ErasureCompleted(result);
	return result;
}

DWORD CErasure::EraseFreeDataSpace(IErasureCallback* callback)
{
	DWORD result;
	UINT64 nFreeSpace;
	UINT64 nFileSize;
	UINT64 nFileCount; //被删除的文件数。（估算）

	nFreeSpace = m_Volume->GetAvailableFreeSpace(&result);
	if (result != 0)
		return result;

	if (nFreeSpace > MAX_TEMPFILESIZE)
		nFileSize = MAX_TEMPFILESIZE;
	else
		nFileSize = nFreeSpace;
	nFileCount = nFreeSpace / nFileSize + 1;  //余数也算一个
	if (m_Volume->GetFileSystem() == FS_NTFS)
		nFileCount++; //NTFS 需擦除MFT中的空闲空间
	nFileCount++;  //删除产生的临时文件算一个

	int nCount = 0;
	if (callback && !callback->ErasureProgress(ERASER_DATA_FREE, nFileCount, nCount++))
		return ERROR_CANCELED;

	while (true)
	{
		nFreeSpace = m_Volume->GetAvailableFreeSpace();
		if(nFreeSpace==0)
			break;
		nFileSize = nFileSize > nFreeSpace ? nFreeSpace : nFileSize;
		
		result = CrateTempFileAndErase(nFileSize, callback);
		if (result != 0)
			break;
		if (callback && !callback->ErasureProgress(ERASER_DATA_FREE, nFileCount, nCount++))
			return ERROR_CANCELED;
	}
	if (result == ERROR_DISK_FULL)
		result = 0;

	if (result == 0)
		result = EraseFreeMftSpace(callback);

	DeleteTempFiles(callback);

	if (callback && !callback->ErasureProgress(ERASER_DATA_FREE, nFileCount, nFileCount))
		return ERROR_CANCELED;
	return result;
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

DWORD CErasure::EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck)
{
	DWORD result = 0;
	PBYTE Buffer = new BYTE[MAX_BUFFER_SIZE];

	for (UINT i = 0; i < m_method->GetPassCount(); i++)
	{
		ErasureMethodPass* pd = m_method->GetPassData(i);
		switch (pd->function)
		{
		case WriteRandom:
			for (int j = 0; j < MAX_BUFFER_SIZE / sizeof(UINT64); j++)
				((PUINT64)Buffer)[j] = Rand64();

			break;
		case WriteConstant:
			for (UINT j = 0; j < MAX_BUFFER_SIZE / pd->nCount; j++)
				memcpy(Buffer + j * pd->nCount, pd->bytes, pd->nCount);

			break;
		default:
			continue;
		}

		result = WriteFileContent(hFile, nStartPos, nFileSize, Buffer, callbck);

		if(result != 0)
			break;
	}
	delete [] Buffer;

	return result;
}

DWORD CErasure::WriteFileContent(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, PBYTE Buffer, IErasureCallback* callbck)
{
	UINT64 nCurrent = 0;

	if (callbck && !callbck->ErasureProgress(ERASER_FILE, nFileSize - nStartPos, nCurrent))
		return ERROR_CANCELED;

	LARGE_INTEGER pos; pos.QuadPart = nStartPos;
	if(SetFilePointerEx(hFile, pos, NULL /*DWORD((nStartPos >> 32) & 0xFFFFFFFF)*/, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return GetLastError();

	DWORD result = 0, dwCb, nBufferSize = (DWORD)min(nFileSize - nStartPos, MAX_BUFFER_SIZE);

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

		if (callbck && !callbck->ErasureProgress(ERASER_FILE, nFileSize-nStartPos, nCurrent))
			return ERROR_CANCELED;
	}

	//if (callbck)
		//callbck->ErasureProgress(ERASER_FILE, nFileSize - nStartPos, nFileSize - nStartPos);

	return result;
}

DWORD CErasure::EraseFreeMftSpace(IErasureCallback* callback)
{
	DWORD result;
	VOLUME_FILE_SYSTEM fs = m_Volume->GetFileSystem(&result);
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
	DWORD dwBytesPreFile = m_Volume->GetBpbData()->BytesPerFileRecordSegment;
	UINT64 nMaxFileCount;
	do 
	{
		nMaxFileCount = max(1, m_Volume->GetRemoveableCount());
		
		if (callback && !callback->ErasureProgress(ERASER_MFT_FREE, nMaxFileCount, 0))
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

			if (callback && !callback->ErasureProgress(ERASER_MFT_FREE, nMaxFileCount, nCount++))
				return ERROR_CANCELED;
		}
	} while (false);

	if (callback && !callback->ErasureProgress(ERASER_MFT_FREE, nMaxFileCount, nMaxFileCount))
		return ERROR_CANCELED;
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
	DWORD result = 0;
	CLdString* tmpName = new CLdString();
	CFileUtils::GenerateRandomFileName(nFileNameLength, tmpName);

	*tmpName = m_tmpDir + *tmpName;
	HANDLE hFile = CreateFile(*tmpName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
			//result = GetLastError();
		}
	}
	if (!pOut)
		CloseHandle(hFile);

	m_Tmpfiles.Add(tmpName);
	return result;
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
	DWORD result;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	result = CreateTempFile(nFileSize, &hFile);
	if (result != 0 && hFile == INVALID_HANDLE_VALUE)
		return result;  
	result = EraseFile(hFile, 0, nFileSize, callback);
	CloseHandle(hFile);
	return result;
}

DWORD CErasure::DeleteTempFiles(IErasureCallback* callback)
{
	DWORD result = 0, nCount = m_Tmpfiles.GetCount();
	if (callback)
		callback->ErasureProgress(ERASER_DEL_TEMPFILE, nCount, 0);

	for (int i = 0; i < m_Tmpfiles.GetCount(); i++)
	{
		HANDLE hFile = CreateFile(*m_Tmpfiles[i], GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			SetEndOfFile(hFile);
			ResetFileDate(hFile);
			CloseHandle(hFile);
		}else
		{
			result = GetLastError();
		}
		
		if (!DeleteFile(*m_Tmpfiles[i]))
			result = GetLastError();
		delete m_Tmpfiles[i];
//		result = CFileUtils::ShDeleteFile(m_Tmpfiles[i]);
		
		if(callback)
			callback->ErasureProgress(ERASER_DEL_TEMPFILE, nCount, i);
	}
	m_Tmpfiles.Clear();

	if (callback)
		callback->ErasureProgress(ERASER_DEL_TEMPFILE, nCount, nCount);
	return result;
}

DWORD CErasure::CreateTempDirectory()
{
	m_tmpDir = m_Volume->GetFullName();
	m_tmpDir += Erasure_temp_path;
	m_tmpDir += _T("\\");
	return CFileUtils::ForceDirectories(m_tmpDir);
}

DWORD CErasure::EraseNtfsTrace(IErasureCallback* callback)
{
	DWORD result;
	UINT64 nMftSize, nCount=0;
	UINT64 totalFiles = 2 * (UINT64)max(1L, m_Volume->GetTrackCount());

	PVOLUME_BPB_DATA pBpb = m_Volume->GetBpbData(&result);
	do
	{
		if (result != 0)
			break;
		nMftSize = pBpb->MftValidDataLength;
		if (result != 0)
			break;
		int pollingInterval = (int)min(max(1, nMftSize / pBpb->BytesPerClusters / 20), 128);

		if (callback && !callback->ErasureProgress(ERASER_DEL_TRACK, totalFiles, 0))
			return ERROR_CANCELED;

		int ntry = 3;
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
				pBpb = m_Volume->RefreshBpbData();
				if (pBpb->MftValidDataLength > nMftSize)
					break;
			}
			if (callback && !callback->ErasureProgress(ERASER_DEL_TRACK, totalFiles, nCount))
				return ERROR_CANCELED;
		}
	} while (false);

	class CErasureCallbackImpl: public IErasureCallback
	{
	public:
		IErasureCallback* callback;
		UINT64 totalFiles, nCount;

		BOOL ErasureStart() override
		{
			return true;
		};
		BOOL ErasureCompleted(DWORD dwErroCode) override
		{
			return true;
		};
		BOOL ErasureProgress(ERASE_STEP nStep, UINT64 nMaxCount, UINT64 nCurent) override
		{
			return callback->ErasureProgress(ERASER_DEL_TRACK, totalFiles, this->nCount + nCurent);
			
		};
	};

	CErasureCallbackImpl impl;
	impl.callback = callback;
	impl.totalFiles = totalFiles;
	impl.nCount = nCount;
	DeleteTempFiles(&impl);

	if (callback)
		callback->ErasureProgress(ERASER_DEL_TRACK, totalFiles, totalFiles);

	return result;

}

DWORD CErasure::EraseFatTrace(IErasureCallback* callback)
{
	CMftReader* reader = CMftReader::CreateReader(m_Volume);
	//reader->SetHolder(this);
	//reader->EnumFiles(nullptr);
	DWORD dwFileCount = 0;
	DWORD result = ((CFatMftReader*)reader)->EraseTrace(dwFileCount);
	delete reader;

	return result;
}

DWORD CErasure::EraseDelFileTrace(IErasureCallback* callback)
{
	DWORD result;
	VOLUME_FILE_SYSTEM fs = m_Volume->GetFileSystem(&result);
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

	DeleteTempFiles(callback);

	return result;

}

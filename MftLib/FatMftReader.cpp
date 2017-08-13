
#include "stdafx.h"
#include "FatMftReader.h"

CFatMftReader::CFatMftReader(void)
{
	ZeroMember();
}


CFatMftReader::~CFatMftReader(void)
{
	if(m_fatCache.pFAT)
		delete m_fatCache.pFAT;
}

bool CFatMftReader::Init()
{
	m_Handle = m_Volume->OpenVolumeHandle();
	m_BpbData = m_Volume->GetBpbData();

	switch(m_Volume->GetFileSystem())
	{
	case FS_FAT12:
		m_EntrySize = 12;
		break;
	case FS_FAT16:
		m_EntrySize = 16;
		break;
	case FS_FAT32:
		m_EntrySize = 32;
		break;
	default:
		return false;
	}
	m_Root.Attr = 0x10;
	switch(m_EntrySize){
	case 12:
	case 16:
		m_Root.ClusterNumberHigh = 0;
		m_Root.ClusterNumberLow = 2;
		break;
	case 32:
		m_Root.ClusterNumberHigh = HIWORD(m_BpbData->RootClus);
		m_Root.ClusterNumberLow = LOWORD(m_BpbData->RootClus);
		break;
	}
	return true;
}

UINT64 CFatMftReader::EnumFiles(UINT_PTR Param)
{
	if (!Init())
		return 0;

	UINT64 result = EnumDirectoryFiles(&m_Root, Param);

	return result;
}

DWORD CFatMftReader::EraseTrace(DWORD& nFileCount)
{
	if (!Init())
		return -1;

	DWORD result = 0;
	//为磁盘擦除设置的特殊值
	if (m_Handle != INVALID_HANDLE_VALUE)
		CloseHandle(m_Handle);
	m_Handle = OpenVolumeWrite();
	if (m_Handle == INVALID_HANDLE_VALUE)
		return 0;
	DWORD CB;
	if (DeviceIoControl(m_Handle, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &CB, nullptr))
	{
		__try
		{
			result = EraseDeleteTrace(&m_Root, nFileCount);
		}
		__finally
		{
			DeviceIoControl(m_Handle, FSCTL_UNLOCK_VOLUME, nullptr, 0, nullptr, 0, &CB, nullptr);
		}
	}

	return result;
}

void CFatMftReader::ZeroMember()
{
	__super::ZeroMember();
	
	m_EntrySize = 0;

	ZeroMemory(&m_Root, sizeof m_Root);
	ZeroMemory(&m_fatCache, sizeof(m_fatCache));
}

//************************************
// Method:    EnumDirectoryFiles
// FullName:  枚举目录下的文件（递归）
// Access:    protected 
// Returns:   UINT64
// Qualifier:
// Parameter: PFAT_FILE pParentDir
//************************************
INT64 CFatMftReader::EnumDirectoryFiles(PFAT_FILE pParentDir, UINT_PTR Param)
{
	INT64 Result = 0;

	DWORD ClusterNumber;    //目录数据起始Cluster
	DWORD BufferLength;
	PFAT_FILE pFatFile;
	DWORD nNamePos = MAX_PATH - 1;           //当前文件名长度(留一个结束符）

	if(m_EntrySize == 32)
		ClusterNumber = MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	else
		ClusterNumber = pParentDir->ClusterNumberLow;

	if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录山区固定
		BufferLength = m_BpbData->SectorsPerRootDirectory * m_BpbData->BytesPerSector;
	else
		BufferLength = m_BpbData->BytesPerClusters;

	PWCHAR FileName = new TCHAR[MAX_PATH]; //长文件名暂存
	PUCHAR Buffer = new UCHAR[BufferLength];
	ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));

	while(ClusterNumber)
	{
		ZeroMemory(Buffer, BufferLength);
		if(m_EntrySize != 32 && pParentDir == &m_Root){
			if(!ReadSector(m_BpbData->FirstDataSector - m_BpbData->SectorsPerRootDirectory, m_BpbData->SectorsPerRootDirectory, Buffer))
				break;
		}else{
			if(!ReadSector(DataClusterStartSector(ClusterNumber), m_BpbData->SectorsPerCluster, Buffer))
				break;
		}
		
		for(DWORD pos=0; pos<BufferLength; pos+=sizeof(FAT_FILE))
		{
			pFatFile = (PFAT_FILE)(Buffer + pos);

			if(pFatFile->Order == 0)  //未使用
				continue;

			switch (pFatFile->Attr) {
			case FFT_VOLUME:
				continue; //卷标
			case FFT_LONGNAME:
				CopyMemory(&FileName[nNamePos - 2], pFatFile->L.Name3, 4);
				CopyMemory(&FileName[nNamePos - 8], pFatFile->L.Name2, 12);
				CopyMemory(&FileName[nNamePos - 13], pFatFile->Name1, 10);
				nNamePos -= 13;
				break;
			default: //FAT 文件记录
				if (nNamePos == MAX_PATH - 1) 
				{  //没有长文件名
					nNamePos--;
					for (int i = 9; i > 6; i--)
						if (pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];

					if (FileName[nNamePos + 1] != 0)
						FileName[nNamePos--] = '.';

					for (int i = 6; i >= 0; i--)
						if (pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];

					FileName[nNamePos] = pFatFile->Order;
				}
			
				if ((FileName[nNamePos] == '.' && FileName[nNamePos + 1] == 0) ||
					(FileName[nNamePos] == '.' && FileName[nNamePos + 1] == '.' && FileName[nNamePos + 2] == 0))
				{
					ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));
					nNamePos = MAX_PATH - 1;
					continue;
				}
				pFatFile->ReferenceNumber = Result++;
				if (!DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir, Param)) 
				{
					Result = -1;
					goto exit;
				}

				if ((pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY)
				{  //子目录递归
					INT64 k = EnumDirectoryFiles(pFatFile, Param);
					if (k == -1)
					{
						Result = -1;
						goto exit;
					}
					else
						Result += k;
				}				
				ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));
				nNamePos = MAX_PATH - 1;
				break;
			}//switch
		}//for

		if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录固定(m_SectorsPerRootDirectory)
			ClusterNumber = 0;
		else
			ClusterNumber = GetNextClusterNumber(ClusterNumber);  //目录下一Cluster数据

	}//while

exit:
	delete FileName;
	delete Buffer;
	//delete aFileInfo;

	return Result;
}

HANDLE CFatMftReader::OpenVolumeWrite()
{
	CLdString path = m_Volume->GetVolumeGuid();
	if (path.IsEmpty())
	{
		path = _T("\\\\.\\");
		path += m_Volume->GetFileName();
		if (path[path.GetLength() - 1] == '\\')
			path.SetAt(path.GetLength() - 1, '\0');
	}
	HANDLE result = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	return result;
}

/*
const PFILE_INFO CFatMftReader::GetFileInfo(UINT64 ReferenceNumber)
{
	return __super::GetFileInfo(ReferenceNumber);	
}
*/

//************************************
// Method:    DataClusterStartSector
// FullName:  转换数据区Cluster号起始扇区号
// Access:    protected 
// Returns:   DWORD
// Qualifier:
// Parameter: DWORD ClusterNumber
//************************************
DWORD CFatMftReader::DataClusterStartSector(DWORD ClusterNumber)
{
	return (ClusterNumber-2) * m_BpbData->SectorsPerCluster + m_BpbData->FirstDataSector;
}

//************************************
// Method:    GetNextClusterNumber
// FullName:  查找下一个数据存储Cluster
// Access:    protected 
// Returns:   DWORD
// Qualifier:
// Parameter: DWORD ClusterNumber
//************************************
DWORD CFatMftReader::GetNextClusterNumber(DWORD ClusterNumber)
{
	DWORD Result = 0;
	UINT64 bitOffset = ClusterNumber * m_EntrySize;
	UINT64 byteOffset = bitOffset / 8; 

	DWORD sectorOffset = (DWORD)(byteOffset / m_BpbData->BytesPerSector);

	PUCHAR pFAT = ReadFat(sectorOffset, 1, TRUE);
	switch(m_EntrySize){
	case 32:
		Result = *(DWORD*)(pFAT+byteOffset %  m_BpbData->BytesPerSector);
		if(Result == EOC32 || Result == BAD32 || Result == RES32)
			Result = 0;
		break;
	case 16:
		Result = *(USHORT*)(pFAT+byteOffset %  m_BpbData->BytesPerSector);
		if(Result == EOC16 || Result == BAD16 || Result == RES16)
			Result = 0;
		break;
	case 12:
		Result = *(DWORD*)(pFAT+byteOffset %  m_BpbData->BytesPerSector);
		Result = Result << bitOffset % 8;
		Result &= 0xFFF00000;
		Result >>= 20;
		if(Result == EOC12 || Result == BAD12 || Result == RES12)
			Result = 0;
		break;
	}

	return Result;
}

//************************************
// Method:    ReadFat
// FullName:  读取FAT扇区数据并缓存
// Access:    protected 
// Returns:   PUCHAR
// Qualifier:
// Parameter: DWORD sector 起始山区
// Parameter: DWORD count  扇区数
// Parameter: BOOL cache  使用缓存数据
//************************************
PUCHAR CFatMftReader::ReadFat(DWORD sector, DWORD count, BOOL cache)
{
	if(cache && m_fatCache.pFAT){
		if(m_fatCache.beginSector <= sector && m_fatCache.beginSector + m_fatCache.sectorCount >= sector + count)
			return m_fatCache.pFAT + (sector - m_fatCache.beginSector)* m_BpbData->BytesPerSector;
	}

	if(m_fatCache.pFAT){
		delete m_fatCache.pFAT;
		m_fatCache.pFAT = NULL;
	}

	PBYTE Buffer = new BYTE[m_BpbData->BytesPerSector * count];
	if(!ReadSector(m_BpbData->FirstFatSector + sector, count, Buffer)){
		delete Buffer;
		return NULL;
	}

	m_fatCache.beginSector = sector;
	m_fatCache.sectorCount = count;
	m_fatCache.pFAT = Buffer;

	return Buffer;
}

BOOL CFatMftReader::DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, UINT_PTR Param)
{
	PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR)];
	ZeroMemory(aFileInfo, sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR));

	if(pParentDir == &m_Root)
		aFileInfo->DirectoryFileReferenceNumber = 0;
	else
		aFileInfo->DirectoryFileReferenceNumber = pFatFile->ReferenceNumber;// MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	aFileInfo->NameLength = (UCHAR)_tcslen(FileName);
	_tcscpy(aFileInfo->Name, FileName);
	aFileInfo->FileAttributes = pFatFile->Attr;
	aFileInfo->DataSize = pFatFile->FileSize;
	if(pFatFile->Attr & FFT_DIRECTORY)
		aFileInfo->AllocatedSize = MAKELONG(pFatFile->ClusterNumberLow, pFatFile->ClusterNumberHigh);
	else{
		ULARGE_INTEGER l = {(DWORD)(pParentDir->ReferenceNumber - pFatFile->ReferenceNumber), (DWORD)MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh)};
		aFileInfo->AllocatedSize = l.QuadPart;
	}

	DosDateTimeToFileTime(pFatFile->CreateDate, pFatFile->CreateTime, &aFileInfo->CreationTime);
	DosDateTimeToFileTime(pFatFile->LastAccessDate, 0, &aFileInfo->LastAccessTime);
	DosDateTimeToFileTime(pFatFile->WriteDate, pFatFile->WriteTime, &aFileInfo->LastWriteTime);
	if (pFatFile->Order == DELETE_FLAG)
		aFileInfo->FileAttributes |= FILE_ATTRIBUTE_DELETED;

	BOOL result = Callback(pFatFile->ReferenceNumber, aFileInfo, Param);

	delete [] (PUCHAR)aFileInfo;

	return result;
}

BOOL CFatMftReader::WriteSector(UINT64 sector, DWORD count, PVOID buffer)
{
	ULARGE_INTEGER offset;
	OVERLAPPED overlap = { 0 };
	DWORD n;
	offset.QuadPart = sector * m_Volume->GetBpbData()->BytesPerSector;
	overlap.Offset = offset.LowPart;
	overlap.OffsetHigh = offset.HighPart;
	return ::WriteFile(m_Handle, buffer, count * m_Volume->GetBpbData()->BytesPerSector, &n, &overlap);
}

/***
擦除删除文件记录。
**/
DWORD CFatMftReader::EraseDeleteTrace(PFAT_FILE pParentDir, DWORD& nFileCount)
{
	DWORD result = 0;

	DWORD ClusterNumber;    //目录数据起始Cluster
	DWORD BufferLength;
	PFAT_FILE pFatFile;

	//获取有Write权限的handle

	if (m_EntrySize == 32)
		ClusterNumber = MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	else
		ClusterNumber = pParentDir->ClusterNumberLow;

	if (m_EntrySize != 32 && pParentDir == &m_Root)  
		BufferLength = m_BpbData->SectorsPerRootDirectory * m_BpbData->BytesPerSector;
	else
		BufferLength = m_BpbData->BytesPerClusters;

	PUCHAR Buffer = new UCHAR[BufferLength];
	PUCHAR back = new UCHAR[BufferLength];
	while (ClusterNumber)
	{
		UINT64 nSector;
		DWORD nSectorCount;
		if (m_EntrySize != 32 && pParentDir == &m_Root) {
			nSector = m_BpbData->FirstDataSector - m_BpbData->SectorsPerRootDirectory;
			nSectorCount = m_BpbData->SectorsPerRootDirectory;
			if (!ReadSector(nSector, nSectorCount, Buffer))
				break;
		}
		else {
			nSector = DataClusterStartSector(ClusterNumber);
			nSectorCount = m_BpbData->SectorsPerCluster;
			if (!ReadSector(nSector, nSectorCount, Buffer))
				break;
		}

		ZeroMemory(back, BufferLength);
		DWORD pos1 = 0;
		for (DWORD pos = 0; pos<BufferLength; pos += sizeof(FAT_FILE))
		{
			pFatFile = (PFAT_FILE)(Buffer + pos);

			if (pFatFile->Order == 0)
				break;
			if (pFatFile->Order == DELETE_FLAG)
			{
				if (!Callback(nFileCount++, nullptr, result))
				{
					result = -1;
					goto exit;
				}

			}else
			{
				CopyMemory(back + pos1, pFatFile, sizeof(FAT_FILE));
				pos1 += sizeof(FAT_FILE);
				if(((pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY) && 
					!((pFatFile->Order == '.' && pFatFile->Name1[0] == ' ') ||
					(pFatFile->Order == '.' && pFatFile->Name1[0] == '.' && pFatFile->Name1[1] == ' ')))
				{
					result = EraseDeleteTrace(pFatFile, nFileCount);
					if (result != 0)
					{
						goto exit;
					}
				}
			}
		}//for

		if (!WriteSector(nSector, nSectorCount, back))
		{
			result = GetLastError();
			break;
		}

		if (m_EntrySize != 32 && pParentDir == &m_Root)  
			ClusterNumber = 0;
		else
			ClusterNumber = GetNextClusterNumber(ClusterNumber); 

	}//while

exit:
	delete Buffer;
	delete back;
	return result;
}

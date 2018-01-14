
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
	PVOLUME_BPB_DATA pBpb = m_Volume->GetVolumeMftData();
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
		m_Root.ClusterNumberHigh = HIWORD(pBpb->RootClus);
		m_Root.ClusterNumberLow = LOWORD(pBpb->RootClus);
		break;
	}
	return true;
}

UINT64 CFatMftReader::EnumFiles(PVOID Param)
{
	if (!Init())
		return 0;

	UINT64 Result = EnumDirectoryFiles(&m_Root, Param);

	return Result;
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
INT64 CFatMftReader::EnumDirectoryFiles(PFAT_FILE pParentDir, PVOID Param)
{
	INT64 Result = 0;

	DWORD ClusterNumber;    //目录数据起始Cluster
	DWORD BufferLength;
	PFAT_FILE pFatFile;
	DWORD nNamePos = MAX_PATH - 1;           //当前文件名长度(留一个结束符）
	PVOLUME_BPB_DATA pBpb = m_Volume->GetVolumeMftData();

	if(m_EntrySize == 32)
		ClusterNumber = MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	else
		ClusterNumber = pParentDir->ClusterNumberLow;

	if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录山区固定
		BufferLength = pBpb->SectorsPerRootDirectory * pBpb->BytesPerSector;
	else
		BufferLength = pBpb->BytesPerClusters;

	PWCHAR FileName = new TCHAR[MAX_PATH]; //长文件名暂存
	PUCHAR Buffer = new UCHAR[BufferLength];
	//PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(FILE_INFO) + MAX_FILE];
	ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));

	while(ClusterNumber){

		ZeroMemory(Buffer, BufferLength);
		if(m_EntrySize != 32 && pParentDir == &m_Root){
			if(!ReadSector(pBpb->FirstDataSector - pBpb->SectorsPerRootDirectory, pBpb->SectorsPerRootDirectory, Buffer))
				break;
		}else{
			if(!ReadSector(DataClusterStartSector(ClusterNumber), pBpb->BytesPerClusters, Buffer))
				break;
		}
		
		for(DWORD pos=0; pos<BufferLength; pos+=sizeof(FAT_FILE)){

			pFatFile = (PFAT_FILE)(Buffer + pos);

			if(pFatFile->Order == 0  //未使用
				|| pFatFile->Order == DELETE_FLAG) //删除
				continue;

			switch(pFatFile->Attr){
			case FFT_VOLUME:
				continue; //卷标
				break;
			case FFT_LONGNAME:
				CopyMemory(&FileName[nNamePos-2], pFatFile->L.Name3, 4);
				CopyMemory(&FileName[nNamePos-8], pFatFile->L.Name2, 12);
				CopyMemory(&FileName[nNamePos-13], pFatFile->Name1, 10);
				nNamePos -= 13;
				break;
			default: //FAT 文件记录
				//ZeroMemory(aFileInfo, sizeof(FILE_INFO) + MAX_FILE);


				if(nNamePos == MAX_PATH - 1){  //没有长文件名

					nNamePos--;

					for(int i=9; i>6; i--)
						if(pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];

					if(FileName[nNamePos+1] != 0)
						FileName[nNamePos--] = '.';


					for(int i=6; i>=0; i--)
						if(pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];

					FileName[nNamePos] = pFatFile->Order;


				}
				
				pFatFile->ReferenceNumber = Result + 1;
				
				if((pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY){  //子目录递归
					if((FileName[nNamePos] == '.' && FileName[nNamePos+1] == 0) || 
						(FileName[nNamePos] == '.' && FileName[nNamePos+1] == '.' && FileName[nNamePos+2] == 0)){
							ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));
							nNamePos = MAX_PATH - 1;
							continue;
					}else{
						INT64 k = EnumDirectoryFiles(pFatFile, Param);
						if(k == -1){
							Result = -1;
							goto exit;
						}else
							Result += k;

					}
				}

				if(!DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir, Param)){
					Result = -1;
					goto exit;
				}else{
					Result ++;
				}

				ZeroMemory(FileName, MAX_PATH * sizeof(TCHAR));
				nNamePos = MAX_PATH - 1;
			}
		}

		if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录固定(m_SectorsPerRootDirectory)
			ClusterNumber = 0;
		else
			ClusterNumber = GetNextClusterNumber(ClusterNumber);  //目录下一Cluster数据

	}

exit:
	delete FileName;
	delete Buffer;
	//delete aFileInfo;

	return Result;
}

const PFILE_INFO CFatMftReader::GetFileInfo(UINT64 ReferenceNumber)
{
	return __super::GetFileInfo(ReferenceNumber);	
}

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
	return (ClusterNumber-2) * m_Volume->GetVolumeMftData()->SectorsPerCluster + m_Volume->GetVolumeMftData()->FirstDataSector;
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

	DWORD sectorOffset = (DWORD)(byteOffset / m_Volume->GetVolumeMftData()->BytesPerSector);

	PUCHAR pFAT = ReadFat(sectorOffset, 1, TRUE);
	switch(m_EntrySize){
	case 32:
		Result = *(DWORD*)(pFAT+byteOffset %  m_Volume->GetVolumeMftData()->BytesPerSector);
		if(Result == EOC32 || Result == BAD32 || Result == RES32)
			Result = 0;
		break;
	case 16:
		Result = *(USHORT*)(pFAT+byteOffset %  m_Volume->GetVolumeMftData()->BytesPerSector);
		if(Result == EOC16 || Result == BAD16 || Result == RES16)
			Result = 0;
		break;
	case 12:
		Result = *(DWORD*)(pFAT+byteOffset %  m_Volume->GetVolumeMftData()->BytesPerSector);
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
			return m_fatCache.pFAT + (sector - m_fatCache.beginSector)* m_Volume->GetVolumeMftData()->BytesPerSector;
	}

	if(m_fatCache.pFAT){
		delete m_fatCache.pFAT;
		m_fatCache.pFAT = NULL;
	}

	PBYTE Buffer = new BYTE[m_Volume->GetVolumeMftData()->BytesPerSector * count];
	if(!ReadSector(m_Volume->GetVolumeMftData()->FirstFatSector + sector, count, Buffer)){
		delete Buffer;
		return NULL;
	}

	m_fatCache.beginSector = sector;
	m_fatCache.sectorCount = count;
	m_fatCache.pFAT = Buffer;

	return Buffer;
}

BOOL CFatMftReader::DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, PVOID Param)
{
	static PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR)];
	ZeroMemory(aFileInfo, sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR));

	if(pParentDir == &m_Root)
		aFileInfo->DirectoryFileReferenceNumber = 0;
	else
		aFileInfo->DirectoryFileReferenceNumber = pFatFile->ReferenceNumber;// MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	aFileInfo->NameLength = (UCHAR)wcslen(FileName);
	wcscat_s(aFileInfo->Name, FileName);
	aFileInfo->FileAttributes = pFatFile->Attr;
	aFileInfo->DataSize = pFatFile->FileSize;
	if(pFatFile->Attr & FFT_DIRECTORY)
		aFileInfo->AllocatedSize = MAKELONG(pFatFile->ClusterNumberLow, pFatFile->ClusterNumberHigh);
	else{
		ULARGE_INTEGER l = {(DWORD)(pParentDir->ReferenceNumber - pFatFile->ReferenceNumber), MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh)};
		aFileInfo->AllocatedSize =  l.QuadPart;
	}

	DosDateTimeToFileTime(pFatFile->CreateDate, pFatFile->CreateTime, &aFileInfo->CreationTime);
	DosDateTimeToFileTime(pFatFile->LastAccessDate, 0, &aFileInfo->LastAccessTime);
	DosDateTimeToFileTime(pFatFile->WriteDate, pFatFile->WriteTime, &aFileInfo->LastWriteTime);

	BOOL result = Callback(pFatFile->ReferenceNumber, aFileInfo, Param);

	delete [] (PUCHAR)aFileInfo;

	return result;
}

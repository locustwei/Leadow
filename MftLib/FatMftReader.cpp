
#include "stdafx.h"
#include "FatMftReader.h"

CFatMftReader::CFatMftReader(HANDLE hVolume)
	:CMftReader(hVolume)
{
	ZeroMember();
	m_Inited = Init();
}


CFatMftReader::~CFatMftReader(void)
{
	if(m_fatCache.pFAT)
		delete m_fatCache.pFAT;
}

bool CFatMftReader::Init()
{
	bool result = __super::Init();


	if(result)
	{
		PUCHAR pBpb = new UCHAR[m_BytesPerSector];
		if(!ReadSector(0, 1, pBpb))
			return false;

		if(((PFAT_BPB)pBpb)->SectorsPerFAT16){
			if(((PFAT_BPB)pBpb)->FAT1216.FilSysType[4] == '2')
				m_EntrySize = 12;
			else
				m_EntrySize = 16;
		}else
			m_EntrySize = 32;

		m_SectorsPerCluster = ((PFAT_BPB)pBpb)->SectorsPerCluster;
		
		m_SectorsPerRootDirectory = (((PFAT_BPB)pBpb)->RootEntriesCount * 0x20 + ((PFAT_BPB)pBpb)->BytesPerSector - 1) / ((PFAT_BPB)pBpb)->BytesPerSector; //FAT16有效

		m_SectorsPerFAT = ((PFAT_BPB)pBpb)->SectorsPerFAT16?((PFAT_BPB)pBpb)->SectorsPerFAT16:((PFAT_BPB)pBpb)->FAT32.SectorsPerFAT32;
		m_FirstDataSector = ((PFAT_BPB)pBpb)->ReservedSectors + ((PFAT_BPB)pBpb)->NumFATs * m_SectorsPerFAT + m_SectorsPerRootDirectory;
		m_FirstFatSector = ((PFAT_BPB)pBpb)->ReservedSectors;
		m_TotalSectors = ((PFAT_BPB)pBpb)->TotalSectors16?((PFAT_BPB)pBpb)->TotalSectors16:((PFAT_BPB)pBpb)->TotalSectors32;

		m_Root.Attr = 0x10;
		switch(m_EntrySize){
		case 12:
		case 16:
			m_Root.ClusterNumberHigh = 0;
			m_Root.ClusterNumberLow = 2;
			break;
		case 32:
			m_Root.ClusterNumberHigh = HIWORD(((PFAT_BPB)pBpb)->FAT32.RootClus);
			m_Root.ClusterNumberLow = LOWORD(((PFAT_BPB)pBpb)->FAT32.RootClus);
			break;
		}

		delete [] pBpb;
	}

	return result;
}

UINT64 CFatMftReader::EnumFiles(IMftReaderHandler* hander, PVOID Param)
{
	__super::EnumFiles(hander, Param);
	
	UINT64 Result = EnumDirectoryFiles(&m_Root);

	return Result;
}

void CFatMftReader::ZeroMember()
{
	__super::ZeroMember();
	
	m_SectorsPerRootDirectory = 0;
	m_SectorsPerFAT = 0;
	m_FirstDataSector = 0;
	m_TotalSectors = 0;
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
LONGLONG CFatMftReader::EnumDirectoryFiles(PFAT_FILE pParentDir)
{
	LONGLONG Result = 0;

	ULONG ClusterNumber;    //目录数据起始Cluster
	ULONG BufferLength;
	PFAT_FILE pFatFile;
	ULONG nNamePos = MAX_PATH - 1;           //当前文件名长度(留一个结束符）


	if(m_EntrySize == 32)
		ClusterNumber = MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	else
		ClusterNumber = pParentDir->ClusterNumberLow;

	if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录山区固定
		BufferLength = m_SectorsPerRootDirectory * m_BytesPerSector;
	else
		BufferLength = m_SectorsPerCluster * m_BytesPerSector;

	PWCHAR FileName = new WCHAR[MAX_PATH]; //长文件名暂存
	PUCHAR Buffer = new UCHAR[BufferLength];
	//PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(MFT_FILE_INFO) + MAX_FILE];
	ZeroMemory(FileName, MAX_PATH * sizeof(WCHAR));

	while(ClusterNumber){

		ZeroMemory(Buffer, BufferLength);
		if(m_EntrySize != 32 && pParentDir == &m_Root){
			if(!ReadSector(m_FirstDataSector - m_SectorsPerRootDirectory, m_SectorsPerRootDirectory, Buffer))
				break;
		}else{
			if(!ReadSector(DataClusterStartSector(ClusterNumber), m_SectorsPerCluster, Buffer))
				break;
		}
		
		for(ULONG pos=0; pos<BufferLength; pos+=sizeof(FAT_FILE)){

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
				//ZeroMemory(aFileInfo, sizeof(MFT_FILE_INFO) + MAX_FILE);


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
				
				if ((FileName[nNamePos] == '.' && FileName[nNamePos + 1] == 0) ||
					(FileName[nNamePos] == '.' && FileName[nNamePos + 1] == '.' && FileName[nNamePos + 2] == 0))
				{
					//.  ..
				}
				else
				{
					DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir);
					Result++;

					if ((pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY) {  //子目录递归
						Result += EnumDirectoryFiles(pFatFile);
					}
				}

				ZeroMemory(FileName, MAX_PATH * sizeof(WCHAR));
				nNamePos = MAX_PATH - 1;
			}
		}

		if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录固定(m_SectorsPerRootDirectory)
			ClusterNumber = 0;
		else
			ClusterNumber = GetNextClusterNumber(ClusterNumber);  //目录下一Cluster数据

	}

	delete FileName;
	delete Buffer;
	//delete aFileInfo;

	return Result;
}

BOOL CFatMftReader::GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo)
{
	return FALSE;	
}

//************************************
// Method:    DataClusterStartSector
// FullName:  转换数据区Cluster号起始扇区号
// Access:    protected 
// Returns:   ULONG
// Qualifier:
// Parameter: ULONG ClusterNumber
//************************************
ULONG CFatMftReader::DataClusterStartSector(ULONG ClusterNumber)
{
	return (ClusterNumber-2) * m_SectorsPerCluster + m_FirstDataSector;
}

//************************************
// Method:    GetNextClusterNumber
// FullName:  查找下一个数据存储Cluster
// Access:    protected 
// Returns:   ULONG
// Qualifier:
// Parameter: ULONG ClusterNumber
//************************************
ULONG CFatMftReader::GetNextClusterNumber(ULONG ClusterNumber)
{
	ULONG Result = 0;
	UINT64 bitOffset = ClusterNumber * m_EntrySize;
	UINT64 byteOffset = bitOffset / 8; 

	ULONG sectorOffset = (ULONG)(byteOffset / m_BytesPerSector);

	PUCHAR pFAT = ReadFat(sectorOffset, 1, TRUE);
	switch(m_EntrySize){
	case 32:
		Result = *(ULONG*)(pFAT+byteOffset % m_BytesPerSector);
		if(Result == EOC32 || Result == BAD32 || Result == RES32)
			Result = 0;
		break;
	case 16:
		Result = *(USHORT*)(pFAT+byteOffset % m_BytesPerSector);
		if(Result == EOC16 || Result == BAD16 || Result == RES16)
			Result = 0;
		break;
	case 12:
		Result = *(ULONG*)(pFAT+byteOffset % m_BytesPerSector);
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
// Parameter: ULONG sector 起始扇区
// Parameter: ULONG count  扇区数
// Parameter: BOOL cache  使用缓存数据
//************************************
PUCHAR CFatMftReader::ReadFat(ULONG sector, ULONG count, BOOL cache)
{
	if(cache && m_fatCache.pFAT){
		if(m_fatCache.beginSector <= sector && m_fatCache.beginSector + m_fatCache.sectorCount >= sector + count)
			return m_fatCache.pFAT + (sector - m_fatCache.beginSector)*m_BytesPerSector;
	}

	if(m_fatCache.pFAT){
		delete m_fatCache.pFAT;
		m_fatCache.pFAT = NULL;
	}

	PBYTE Buffer = new BYTE[m_BytesPerSector * count];
	if(!ReadSector(m_FirstFatSector + sector, count, Buffer)){
		delete Buffer;
		return NULL;
	}

	m_fatCache.beginSector = sector;
	m_fatCache.sectorCount = count;
	m_fatCache.pFAT = Buffer;

	return Buffer;
}

BOOL CFatMftReader::DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir)
{
	static UINT filecount = 15;  //为ntfs兼容，ID从6开始，ntfs根目录为5, $Extend为11

	PMFT_FILE_DATA aFileInfo =(PMFT_FILE_DATA) new UCHAR[sizeof(MFT_FILE_DATA) + MAX_PATH * sizeof(WCHAR)];
	ZeroMemory(aFileInfo, sizeof(MFT_FILE_DATA) + MAX_PATH * sizeof(WCHAR));

	if(pParentDir == &m_Root)
		aFileInfo->DirectoryFileReferenceNumber = 0;
	else
		aFileInfo->DirectoryFileReferenceNumber = pParentDir->ReferenceNumber;// MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);

	aFileInfo->NameLength = (UCHAR)wcslen(FileName);
	wcscat_s(aFileInfo->Name, FileName);
	aFileInfo->FileAttributes = pFatFile->Attr;
	aFileInfo->DataSize = pFatFile->FileSize;
	//ULARGE_INTEGER l = {  pFatFile->ReferenceNumber, MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh) };
	aFileInfo->FileReferenceNumber = filecount++;
	pFatFile->ReferenceNumber = aFileInfo->FileReferenceNumber;

	DosDateTimeToFileTime(pFatFile->CreateDate, pFatFile->CreateTime, &aFileInfo->CreationTime);
	//DosDateTimeToFileTime(pFatFile->LastAccessDate, 0, &aFileInfo->LastAccessTime);
	DosDateTimeToFileTime(pFatFile->WriteDate, pFatFile->WriteTime, &aFileInfo->LastWriteTime);

	BOOL result = Callback(aFileInfo->FileReferenceNumber, aFileInfo);

	delete [] (PUCHAR)aFileInfo;

	return result;
}

CMftFile* CFatMftReader::GetFile(UINT64 FileNumber, bool OnlyName /*= false*/)
{
	return nullptr;
}

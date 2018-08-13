
#include "stdafx.h"
#include "FatMftReader.h"

CFatMftReader::CFatMftReader(HANDLE hVolume)
	:CMftReader(hVolume)
{
	ZeroMember();
	//m_Inited = Init();
}


CFatMftReader::~CFatMftReader(void)
{
	ClearCatch();
}

BOOL CFatMftReader::ReadCluster(UINT64 Cluster, UINT count, PVOID buffer)
{
	return ReadSector(DataClusterStartSector(Cluster), m_SectorsPerCluster * count, buffer);
}

bool CFatMftReader::Init()
{
	ClearCatch();

	if (m_Inited)
		return true;

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
		m_TotalCluster = m_TotalSectors / m_SectorsPerCluster;
		//m_RootPath.Load();

		m_Root.Attr = 0x10;
		m_Root.IsDeleted = 0;

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
	
	return EnumDirectoryFiles(&m_Root);
}

UINT64 CFatMftReader::EnumDeleteFiles(IMftDeleteReaderHandler* hanlder, PVOID Param)
{
	__super::EnumDeleteFiles(hanlder, Param);

//	EnumDirectoryFiles(&m_Root, 1);
	UINT64 result = EnumDirectoryFiles(&m_Root, 2);

	return result;
}

void CFatMftReader::ZeroMember()
{
	__super::ZeroMember();
	
	m_SectorsPerRootDirectory = 0;
	m_SectorsPerFAT = 0;
	m_FirstDataSector = 0;
	m_TotalSectors = 0;
	m_EntrySize = 0;
	m_TotalCluster = 0;

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
INT64 CFatMftReader::EnumDirectoryFiles(PFAT_FILE pParentDir, int op)
{
	INT64 Result = 0;

	UINT ClusterNumber;    //目录数据起始Cluster
	UINT ClusterCount = 1;     //连续Cluster 
	UINT BufferLength;
	PFAT_FILE pFatFile;
	int nNamePos = MAX_PATH ;           //当前文件名长度(留一个结束符）


	if(m_EntrySize == 32)
		ClusterNumber = MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);
	else
		ClusterNumber = pParentDir->ClusterNumberLow;


	if(m_EntrySize != 32 && pParentDir == &m_Root)  //FAT16根目录扇区固定
		BufferLength = m_SectorsPerRootDirectory * m_BytesPerSector;
	else
		BufferLength = m_SectorsPerCluster * m_BytesPerSector;

	PWCHAR FileName = new WCHAR[MAX_PATH+1]; //长文件名暂存
	PUCHAR Buffer = new UCHAR[BufferLength];
	//PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(MFT_FILE_INFO) + MAX_FILE];
	ZeroMemory(FileName, (MAX_PATH+1) * sizeof(WCHAR));

	while(ClusterNumber){
		ZeroMemory(Buffer, BufferLength);
		if(m_EntrySize != 32 && pParentDir == &m_Root){
			if(!ReadSector(m_FirstDataSector - m_SectorsPerRootDirectory, m_SectorsPerRootDirectory, Buffer))
				break;
		}else{
			if(!ReadCluster(ClusterNumber, ClusterCount, Buffer))
				break;
		}
		
		UCHAR Checksum; //长文件名校验。

		for(UINT pos=0; pos<BufferLength; pos+=sizeof(FAT_FILE)){

			pFatFile = (PFAT_FILE)(Buffer + pos);

			if (pFatFile->Order == 0)
				break;

			if(op != 2 && pFatFile->Order == DELETE_FLAG) 
				continue;

			switch(pFatFile->Attr){
			case FFT_VOLUME:
				continue; //卷标
				//break;
			case FFT_LONGNAME:
				if (nNamePos == MAX_PATH)
					Checksum = pFatFile->L.Checksum;
				else if (pFatFile->L.Checksum != Checksum)
				{
					ZeroMemory(FileName, (MAX_PATH + 1) * sizeof(WCHAR));
					nNamePos = MAX_PATH;
					continue;//校验错误
				}

				CopyMemory(&FileName[nNamePos-2], pFatFile->L.Name3, 4);
				CopyMemory(&FileName[nNamePos-8], pFatFile->L.Name2, 12);
				CopyMemory(&FileName[nNamePos-13], pFatFile->Name1, 10);
				nNamePos -= 13;
				if (nNamePos <= 0)   
					continue;    //错误
				break;
			default: //FAT 文件记录
				//ZeroMemory(aFileInfo, sizeof(MFT_FILE_INFO) + MAX_FILE);

				if (pParentDir->Order == DELETE_FLAG)  //在已删除的文件夹里找文件存在很大几率是“脏读”，这里特殊处理一下
				{
					//if ((pFatFile->Attr & 0x7) == 0 //- 只读  
					//	//&& pFatFile->Attr != 0x02 //- 隐藏  
					//	//&& pFatFile->Attr != 0x04 //- 系统文件  
					//	&& pFatFile->Attr != 0x10 //- 目录 
					//	&& pFatFile->Attr != 0x20 //- 存档
					//	)
					//{
					//	ZeroMemory(FileName, (MAX_PATH + 1) * sizeof(WCHAR));
					//	nNamePos = MAX_PATH;
					//	continue;
					//}
				}

				if(nNamePos == MAX_PATH){  //没有长文件名

					nNamePos--;

					for(int i=9; i>6; i--)
						if(pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];

					if(FileName[nNamePos+1] != 0)
						FileName[nNamePos--] = '.';


					for(int i=6; i>=0; i--)
						if(pFatFile->Name1[i] != 0x20 && pFatFile->Name1[i] != 0)
							FileName[nNamePos--] = pFatFile->Name1[i];
					if(pFatFile->Order == DELETE_FLAG)
						FileName[nNamePos] = '~';
					else
						FileName[nNamePos] = pFatFile->Order;


				}
				
				if ((FileName[nNamePos] == '.' && FileName[nNamePos + 1] == 0) ||
					(FileName[nNamePos] == '.' && FileName[nNamePos + 1] == '.' && FileName[nNamePos + 2] == 0))
				{
					//.  ..
				}
				else
				{
					switch (op)
					{
					case 0:
						DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir);
						break;
					case 1:
						//DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir);
						break;
					case 2:
						if (pParentDir->IsDeleted)
							pFatFile->IsDeleted = 1;
						else
							pFatFile->IsDeleted = (pFatFile->Order == DELETE_FLAG);

						if (pFatFile->IsDeleted || (pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY)
							DoAFatFile(pFatFile, &FileName[nNamePos], pParentDir, true);
					default:
						break;
					}
					

					Result++;

					if ((pFatFile->Attr & FFT_DIRECTORY) == FFT_DIRECTORY) {  //子目录递归
						Result += EnumDirectoryFiles(pFatFile, op);
					}
				}

				ZeroMemory(FileName, (MAX_PATH+1) * sizeof(WCHAR));
				nNamePos = MAX_PATH;
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
// Returns:   UINT
// Qualifier:
// Parameter: UINT ClusterNumber
//************************************
UINT CFatMftReader::DataClusterStartSector(UINT ClusterNumber)
{
	return (ClusterNumber-2) * m_SectorsPerCluster + m_FirstDataSector;
}

//************************************
// Method:    GetNextClusterNumber
// FullName:  查找下一个数据存储Cluster
// Access:    protected 
// Returns:   UINT
// Qualifier:
// Parameter: UINT ClusterNumber
//************************************
UINT CFatMftReader::GetNextClusterNumber(UINT ClusterNumber, PUINT count)
{
	UINT Result = 0;
	UINT64 bitOffset = ClusterNumber * m_EntrySize;
	UINT64 byteOffset = bitOffset / 8; 

	UINT sectorOffset = (UINT)(byteOffset / m_BytesPerSector);

	PUCHAR pFAT = ReadFat(sectorOffset, 1024, TRUE);
	switch(m_EntrySize){
	case 32:
		Result = *(UINT*)(pFAT+byteOffset % m_BytesPerSector);
		if(Result == EOC32 || Result == BAD32 || Result == RES32)
			Result = 0;
		break;
	case 16:
		Result = *(USHORT*)(pFAT+byteOffset % m_BytesPerSector);
		if(Result == EOC16 || Result == BAD16 || Result == RES16)
			Result = 0;
		break;
	case 12:
		Result = *(UINT*)(pFAT+byteOffset % m_BytesPerSector);
		Result = Result << bitOffset % 8;
		Result &= 0xFFF00000;
		Result >>= 20;
		if(Result == EOC12 || Result == BAD12 || Result == RES12)
			Result = 0;
		break;
	}

	if (count && Result && Result - ClusterNumber == 1)
	{
		GetNextClusterNumber(Result, count);
	}
	return Result;
}

//************************************
// Method:    ReadFat
// FullName:  读取FAT扇区数据并缓存
// Access:    protected 
// Returns:   PUCHAR
// Qualifier:
// Parameter: UINT sector 起始扇区
// Parameter: UINT count  扇区数
// Parameter: BOOL cache  使用缓存数据
//************************************
PUCHAR CFatMftReader::ReadFat(UINT sector, UINT count, BOOL cache)
{
	if(cache && m_fatCache.pFAT){
		if(m_fatCache.beginSector <= sector && m_fatCache.beginSector + m_fatCache.sectorCount >= sector + count)
			return m_fatCache.pFAT + (sector - m_fatCache.beginSector)*m_BytesPerSector;
	}

	if(m_fatCache.pFAT){
		delete m_fatCache.pFAT;
		m_fatCache.pFAT = NULL;
	}
	if (sector + count > m_SectorsPerFAT)
		count = m_SectorsPerFAT - sector;

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

BOOL CFatMftReader::DoAFatFile(PFAT_FILE pFatFile, PWCHAR FileName, PFAT_FILE pParentDir, bool deleted)
{
	static UINT FileReferenceNumber = 15;  //为ntfs兼容，ID从15开始，ntfs根目录为5, $Extend为11

	static MFT_FILE_DATA aFileInfo;// = (PMFT_FILE_DATA) new UCHAR[sizeof(MFT_FILE_DATA) + MAX_PATH * sizeof(WCHAR)];

	ZeroMemory(&aFileInfo, sizeof(MFT_FILE_DATA) + MAX_PATH * sizeof(WCHAR));

	if(pParentDir == &m_Root)
		aFileInfo.DirectoryFileReferenceNumber = 5;
	else
		aFileInfo.DirectoryFileReferenceNumber = pParentDir->ReferenceNumber;// MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh);

	aFileInfo.NameLength = (UCHAR)wcslen(FileName);
	wcscat_s(aFileInfo.Name, FileName);
	aFileInfo.FileAttributes = pFatFile->Attr;
	aFileInfo.DataSize = pFatFile->FileSize;
	//ULARGE_INTEGER l = {  pFatFile->ReferenceNumber, MAKELONG(pParentDir->ClusterNumberLow, pParentDir->ClusterNumberHigh) };
	aFileInfo.FileReferenceNumber = FileReferenceNumber++;
	pFatFile->ReferenceNumber = aFileInfo.FileReferenceNumber;

	DosDateTimeToFileTime(pFatFile->CreateDate, pFatFile->CreateTime, &aFileInfo.CreationTime);
	//DosDateTimeToFileTime(pFatFile->LastAccessDate, 0, &aFileInfo->LastAccessTime);
	DosDateTimeToFileTime(pFatFile->WriteDate, pFatFile->WriteTime, &aFileInfo.LastWriteTime);

	if (deleted)
	{
		FILE_DATA_STREAM aDataStream = { 0 };
		//删除文件，查看数据是否被覆盖。事实上文件删除时数据cluster链已经被清0，所以只能确认第一个数据cluster是否被覆盖。
		//这里把连续的cluster认作删除文件的数据是错误的做法。
		if ((pFatFile->Attr & 0x10) == 0)
		{
			UINT ClusterNumber;
			if (m_EntrySize == 32)
				ClusterNumber = MAKELONG(pFatFile->ClusterNumberLow, pFatFile->ClusterNumberHigh);
			else
				ClusterNumber = pFatFile->ClusterNumberLow;
			if (ClusterNumber <= 2 || ClusterNumber > m_TotalCluster)
				return TRUE;  //脏读 或者是重命名文件名。

			aDataStream.Size = pFatFile->FileSize;
			aDataStream.LcnCount = pFatFile->FileSize / (m_BytesPerSector * m_SectorsPerCluster);
			if (aDataStream.LcnCount * m_BytesPerSector * m_SectorsPerCluster < pFatFile->FileSize)
				aDataStream.LcnCount++;
			aDataStream.Lcns = new UINT64[aDataStream.LcnCount];


			for (UINT i = 0; i < aDataStream.LcnCount; i++)
			{
				aDataStream.Lcns[i] = ClusterNumber + i;

				if (GetNextClusterNumber(ClusterNumber + i) != 0)
					aDataStream.Lcns[i] |= 0x8000000000000000;
			}

		}
		BOOL result = DelCallback(FileReferenceNumber, &aFileInfo, &aDataStream);
		if (aDataStream.Lcns)
			delete[]aDataStream.Lcns;
		return result;
	}
	else
		return Callback(aFileInfo.FileReferenceNumber, &aFileInfo);
}

CMftFile* CFatMftReader::GetFile(UINT64 FileNumber, bool OnlyName /*= false*/)
{
	return nullptr;
}

void CFatMftReader::ClearCatch()
{
	if (m_fatCache.pFAT)
		delete m_fatCache.pFAT;
	m_fatCache.pFAT = nullptr;
	m_fatCache.beginSector = 0;
	m_fatCache.sectorCount = 0;
}



#include "stdafx.h"
#include "exFatReader.h"

#define MAX_BUFFER_SIZE 1024 * 1024 * 5

CExFatReader::CExFatReader(HANDLE hVolume)
	:CMftReader(hVolume)
{
	ZeroMember();
	//m_Inited = Init();
}


CExFatReader::~CExFatReader(void)
{
	ClearCatch();
}

BOOL CExFatReader::ReadCluster(UINT64 Cluster, UINT count, PVOID buffer)
{
	return ReadSector(DataClusterStartSector((UINT)Cluster), m_SectorsPerCluster * count, buffer);
}

bool CExFatReader::Init()
{
	ClearCatch();

	if (m_Inited)
		return true;

	bool result = __super::Init();


	if(result)
	{
		PFAT_BPB pBpb = (PFAT_BPB)new UCHAR[m_BytesPerSector];
		if(!ReadSector(0, 1, pBpb))
			return false;

		m_SectorsPerCluster = 1 << (pBpb->EXFAT.SectorsPerCluster);
		
		m_SectorsPerFAT = pBpb->EXFAT.FATSectors;
		m_FirstDataSector = pBpb->EXFAT.FirstClusterSector;
		m_FirstFatSector = pBpb->EXFAT.FATStartSector;
		m_TotalSectors = pBpb->EXFAT.TotalSectors;
		m_TotalCluster = pBpb->EXFAT.ToltalClusters;

		m_Root.CUSTOM.fraFlag = 0x1;
		m_Root.CUSTOM.ReferenceNumber = 5;
		m_Root.CUSTOM.StartCluster = pBpb->EXFAT.RootClus;
		m_Root.CUSTOM.Size = m_BytesPerSector * m_SectorsPerCluster;
		m_Root.CUSTOM.IsDel = false;

		delete [] pBpb;
	}

	return result;
}

UINT64 CExFatReader::EnumFiles(IMftReaderHandler* hander, PVOID Param)
{
	__super::EnumFiles(hander, Param);
	
	return EnumDirectoryFiles(&m_Root);
}

UINT64 CExFatReader::EnumDeleteFiles(IMftDeleteReaderHandler* hanlder, PVOID Param)
{
	__super::EnumDeleteFiles(hanlder, Param);

//	EnumDirectoryFiles(&m_Root, 1);
	UINT64 result = EnumDirectoryFiles(&m_Root, 2);

	return result;
}

BOOL CExFatReader::GetFileStats(PUINT64 FileCount, PUINT64 FolderCount, PUINT64 DeletedFileTracks)
{
	__super::GetFileStats(FileCount, FolderCount, DeletedFileTracks);
	m_FileCount_Stats = FileCount;
	m_FolderCount_Stats = FolderCount;
	m_DeleteFileTracks_Stats = DeletedFileTracks;

	UINT64 result = EnumDirectoryFiles(&m_Root, 1);
	return result > 0;
}

void CExFatReader::ZeroMember()
{
	__super::ZeroMember();
	
	//m_SectorsPerRootDirectory = 0;
	m_SectorsPerFAT = 0;
	m_FirstDataSector = 0;
	m_TotalSectors = 0;
	m_TotalCluster = 0;
	m_ClusterBitmapStartSector = 0;
	m_ClusterBitmapSectors = 0;
	m_FileReferenceNumber = 15;  //为ntfs兼容，ID从15开始，ntfs根目录为5, $Extend为11

	ZeroMemory(&m_BitmapCache, sizeof(m_BitmapCache));
	ZeroMemory(&m_Root, sizeof(m_Root));
	ZeroMemory(&m_fatCache, sizeof(m_fatCache));
	ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
}

//************************************
// Method:    EnumDirectoryFiles
// FullName:  枚举目录下的文件（递归）
// Access:    protected 
// Returns:   UINT64
// Qualifier:
// Parameter: PEXFAT_FILE pParentDir
//************************************
INT64 CExFatReader::EnumDirectoryFiles(PEXFAT_FILE pParentDir, int op)
{
	INT64 Result = 0;

	UINT ClusterNumber;    //目录数据起始Cluster
	UINT ClusterCount = 1;     //连续Cluster 
	UINT BufferLength;
	PEXFAT_FILE pFatFile;
	int nNamePos = MAX_PATH ;           //当前文件名长度(留一个结束符）
	EXFAT_FILE tmpDir;      
	PEXFAT_FILE tmpFile2;

	if (op != 0 && pParentDir->CUSTOM.IsDel)
	{
		if (IsClusterUsed(pParentDir->CUSTOM.StartCluster))
			return 0;
	}
	ClusterNumber = pParentDir->CUSTOM.StartCluster;
	if(pParentDir->CUSTOM.fraFlag == 0x1)
		BufferLength = m_SectorsPerCluster * m_BytesPerSector;
	else
	{
		ClusterCount = MAX_BUFFER_SIZE / (m_SectorsPerCluster * m_BytesPerSector);   //10M占几个簇？
		if (ClusterCount == 0)
			ClusterCount = 1;

		if (pParentDir->CUSTOM.Size < ClusterCount * m_SectorsPerCluster * m_BytesPerSector)
		{
			ClusterCount = pParentDir->CUSTOM.Size / (m_SectorsPerCluster * m_BytesPerSector);
			if (ClusterCount * m_SectorsPerCluster * m_BytesPerSector < pParentDir->CUSTOM.Size)
				ClusterCount++;
		}
		BufferLength = ClusterCount * m_SectorsPerCluster * m_BytesPerSector;
	}
	PUCHAR Buffer = new UCHAR[BufferLength];

	while(ClusterNumber){
		//ZeroMemory(Buffer, BufferLength);

		if (!ReadCluster(ClusterNumber, ClusterCount, Buffer))
			break;
		
		for(UINT pos=0; pos<BufferLength; pos+=sizeof(EXFAT_FILE)){

			pFatFile = (PEXFAT_FILE)(Buffer + pos);

			if (pFatFile->type == 0)
				break;

			//if(op != 2 && pFatFile->type == DELETE_FLAG) 
			//	continue;

			switch(pFatFile->type){
			case 0x3:
			case 0x83:
				continue; //卷标
			case 0x81:
				m_ClusterBitmapStartSector = DataClusterStartSector(pFatFile->CLUBIT.StartCluster);
				m_ClusterBitmapSectors = pFatFile->CLUBIT.Size / m_BytesPerSector;
				continue; //族位图
			case 0x82:
				continue; //大写字符文件
			case 0x5:        //删除
				if (op == 0)
					continue;
			case 0x85:
				DosDateTimeToFileTime(LOWORD(pFatFile->FILE1.CreateTime), HIWORD(pFatFile->FILE1.CreateTime), &m_FileInfo.CreationTime);
				DosDateTimeToFileTime(LOWORD(pFatFile->FILE1.LastMotifyTime), HIWORD(pFatFile->FILE1.LastMotifyTime), &m_FileInfo.LastWriteTime);
				m_FileInfo.FileAttributes = pFatFile->FILE1.Attribute;
				break;    //目录
			case 0x40:      //删除文件属性
				if (op == 0)
					continue;
			case 0xC0:      //正常文件属性
				m_FileInfo.NameLength = pFatFile->FILE2.NameLength;
				m_FileInfo.DataSize = pFatFile->FILE2.Size;

				nNamePos = m_FileInfo.NameLength;
				if (m_FileInfo.FileAttributes & FFT_DIRECTORY)
				{
					tmpDir.CUSTOM.StartCluster = pFatFile->FILE2.ClusterNum;
					tmpDir.CUSTOM.fraFlag = pFatFile->FILE2.fraFlag;
					tmpDir.CUSTOM.Size = pFatFile->FILE2.Size;
				}
				tmpFile2 = pFatFile;
				break;
			case 0x41:        //删除文件名
				if (op == 0)
					continue;
			case 0xC1:        //正常文件名
				//CopyMemory(&m_FileInfo.Name[m_FileInfo.NameLength - nNamePos], pFatFile->DIR_ATTRIBUTE3.Name, (nNamePos >= 15?15:nNamePos) * sizeof(WCHAR));

				if (nNamePos >= 15)
				{
					CopyMemory(&m_FileInfo.Name[m_FileInfo.NameLength - nNamePos], pFatFile->FILE3.Name, 15 * sizeof(WCHAR));
					nNamePos -= 15;
				}
				else
				{
					CopyMemory(&m_FileInfo.Name[m_FileInfo.NameLength - nNamePos], pFatFile->FILE3.Name, nNamePos * sizeof(WCHAR));
					nNamePos = 0;
				}
				if (nNamePos == 0)
				{
					m_FileInfo.Name[m_FileInfo.NameLength] = '\0';
					tmpDir.CUSTOM.IsDel = pFatFile->type == 0x41;

					if((op == 0 && !tmpDir.CUSTOM.IsDel) ||
						(op == 1) ||
						((op == 2 && tmpDir.CUSTOM.IsDel) || m_FileInfo.FileAttributes & FFT_DIRECTORY) 
						)
						DoAFatFile(pParentDir, tmpFile2, op);

					if ((m_FileInfo.FileAttributes & FFT_DIRECTORY) == FFT_DIRECTORY)
					{
						tmpDir.CUSTOM.ReferenceNumber = m_FileInfo.FileReferenceNumber;

						Result += EnumDirectoryFiles(&tmpDir, op);
					}
				}
				break;
			default:	
				break;
			}
		}

		if(pParentDir->CUSTOM.fraFlag == 0x1)
			ClusterNumber = GetNextClusterNumber(ClusterNumber);  //目录下一Cluster数据
		else
		{
			if (pParentDir->CUSTOM.Size > BufferLength)
			{
				pParentDir->CUSTOM.Size -= BufferLength;
				ClusterNumber += ClusterCount;;

				if (pParentDir->CUSTOM.Size < ClusterCount * m_SectorsPerCluster * m_BytesPerSector)
				{
					ClusterCount = pParentDir->CUSTOM.Size / (m_SectorsPerCluster * m_BytesPerSector);
					if (ClusterCount * m_SectorsPerCluster * m_BytesPerSector < pParentDir->CUSTOM.Size)
						ClusterCount++;
				}
				BufferLength = ClusterCount * m_SectorsPerCluster * m_BytesPerSector;

			}
			else
				ClusterNumber = 0;
		}
	}

	delete Buffer;

	return Result;
}

BOOL CExFatReader::GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo)
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
UINT64 CExFatReader::DataClusterStartSector(UINT ClusterNumber)
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
UINT CExFatReader::GetNextClusterNumber(UINT ClusterNumber)
{
	UINT Result = 0;
	UINT64 byteOffset = ClusterNumber * 4;

	UINT sectorOffset = (UINT)(byteOffset / m_BytesPerSector);

	if (m_fatCache.buffer && m_fatCache.beginSector <= sectorOffset && m_fatCache.beginSector + m_fatCache.sectorCount > sectorOffset)
	{
		Result = *(UINT*)(m_fatCache.buffer + (sectorOffset - m_fatCache.beginSector) * m_BytesPerSector + byteOffset % m_BytesPerSector);
	}
	else
	{
		PUCHAR pFAT = CacheFat(sectorOffset, 1024);
		if (pFAT)
			Result = *(UINT*)(pFAT + byteOffset % m_BytesPerSector);
	}
	if (Result == 0xFFFFFFFF)
		Result = 0;
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
PUCHAR CExFatReader::CacheFat(UINT sector, UINT count)
{
	if(m_fatCache.buffer){
		delete m_fatCache.buffer;
		m_fatCache.buffer = NULL;
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
	m_fatCache.buffer = Buffer;

	return Buffer;
}

BOOL CExFatReader::DoAFatFile(PEXFAT_FILE pParentDir, PEXFAT_FILE pFile, int op)
{
	m_FileInfo.DirectoryFileReferenceNumber = pParentDir->CUSTOM.ReferenceNumber;

	m_FileInfo.FileReferenceNumber = m_FileReferenceNumber++;


	if (op == 2)
	{
		FILE_DATA_STREAM aDataStream = { 0 };

		if ((m_FileInfo.FileAttributes & FFT_DIRECTORY) == 0)
		{
			UINT ClusterNumber = pFile->FILE2.ClusterNum;

			if (ClusterNumber <= 2 || ClusterNumber > m_TotalCluster)
				return TRUE;  //脏读 或者是重命名文件名。

			aDataStream.Size = m_FileInfo.DataSize;
			aDataStream.LcnCount = m_FileInfo.DataSize / (m_BytesPerSector * m_SectorsPerCluster);
			if (aDataStream.LcnCount * m_BytesPerSector * m_SectorsPerCluster < m_FileInfo.DataSize)
				aDataStream.LcnCount++;
			aDataStream.Lcns = new UINT64[aDataStream.LcnCount];


			for (UINT i = 0; i < aDataStream.LcnCount; i++)
			{
				if (ClusterNumber == 0 || IsClusterUsed(ClusterNumber))
				{
					aDataStream.Lcns[i] = ClusterNumber | 0x8000000000000000;
				}
				else
					aDataStream.Lcns[i] = ClusterNumber;

				if (pFile->FILE2.fraFlag == 0x3)
					ClusterNumber = ClusterNumber + i;
				else
					ClusterNumber = 0;       //FAT在文件删除时已经置0
			}

		}
		BOOL result = DelCallback(m_FileReferenceNumber, &m_FileInfo, &aDataStream);
		if (aDataStream.Lcns)
			delete[]aDataStream.Lcns;
		return result;
	}
	else if (op == 1)
	{
		if (pFile->CUSTOM.IsDel)
		{
			if (m_DeleteFileTracks_Stats)
				*m_DeleteFileTracks_Stats++;
		}
		else
		{
			if ((m_FileInfo.FileAttributes & FFT_DIRECTORY) == FFT_DIRECTORY)
			{
				if (m_FolderCount_Stats)
					*m_FolderCount_Stats++;
			}
			else
				if (m_FileCount_Stats)
					*m_FileCount_Stats++;
		}

	}
	else if(op == 0)
		return Callback(m_FileInfo.FileReferenceNumber, &m_FileInfo);
}

CMftFile* CExFatReader::GetFile(UINT64 FileNumber, bool OnlyName /*= false*/)
{
	return nullptr;
}

void CExFatReader::ClearCatch()
{
	if (m_fatCache.buffer)
		delete m_fatCache.buffer;
	ZeroMemory(&m_fatCache, sizeof(m_fatCache));

	if (m_BitmapCache.buffer)
		delete m_BitmapCache.buffer;
	ZeroMemory(&m_BitmapCache, sizeof(m_BitmapCache));
}

PUCHAR CExFatReader::CactchClusterBitmap(UINT sectorOffset)
{
	UINT64 StartSector = m_ClusterBitmapStartSector + sectorOffset;
	UINT maxbuffer = MAX_BUFFER_SIZE;
	if (m_SectorsPerCluster * m_BytesPerSector > maxbuffer)
		maxbuffer = m_SectorsPerCluster * m_BytesPerSector;

	UINT sectorcount = maxbuffer / m_BytesPerSector;
	if (m_ClusterBitmapSectors < (sectorcount + sectorOffset))
	{
		sectorcount = m_ClusterBitmapSectors - sectorOffset;
	}

	if (m_BitmapCache.buffer)
		delete[] m_BitmapCache.buffer;

	m_BitmapCache.buffer = new BYTE[sectorcount * m_BytesPerSector];
	if (!ReadSector(m_ClusterBitmapStartSector + sectorOffset, sectorcount, m_BitmapCache.buffer))
	{
		delete[] m_BitmapCache.buffer;
		m_BitmapCache.buffer = nullptr;
	}
	m_BitmapCache.beginSector = sectorOffset;
	m_BitmapCache.sectorCount = sectorcount;
	return m_BitmapCache.buffer;
}

bool CExFatReader::IsClusterUsed(UINT ClusterNumber)
{
	UCHAR Result = 0;
	UINT64 byteOffset = ClusterNumber / 8;

	UINT sectorOffset = (UINT)(byteOffset / m_BytesPerSector);

	if (m_BitmapCache.buffer && m_BitmapCache.beginSector <= sectorOffset && m_BitmapCache.beginSector + m_BitmapCache.sectorCount > sectorOffset)
	{
		Result = *(PUCHAR)(m_BitmapCache.buffer + (sectorOffset - m_BitmapCache.beginSector) * m_BytesPerSector + byteOffset % m_BytesPerSector);
	}
	else
	{
		PUCHAR pFAT = CactchClusterBitmap(sectorOffset);
		if (pFAT)
			Result = *(PUCHAR)(pFAT + byteOffset);
	}
	return Result & (1 << ClusterNumber % 8);

}


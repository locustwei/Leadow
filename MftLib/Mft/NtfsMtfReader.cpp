#include "stdafx.h"
#include "NtfsMtfReader.h"
#include "NtfsFile.h"

#define MAX_CACHE_SECTORS  4000

template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

CNtfsMtfReader::CNtfsMtfReader(HANDLE hVolume)
	:CMftReader(hVolume)
	,m_MftFile(this)
	,m_ClusterBitmap(this)
{
	ZeroMember();
	//m_Inited = Init();
}


CNtfsMtfReader::~CNtfsMtfReader(void)
{
	//if(m_MftRecord)
	//	delete m_MftRecord;
	//m_MftRecord = NULL;
	if(m_MftBitmap)
		delete m_MftBitmap;
	m_MftBitmap = NULL;

	if(!m_FileCanche)
		delete m_FileCanche;
	m_FileCanche = NULL;
	
	ClearCache();
}

/*
0   $MFT
1   $MFTMirr
2   $LogFile
3   $Volume
4   $AttrDef
5   .
6   $Bitmap
7   $Boot
8   $BadClus
9   $Secure
10   $UpCase
11   $Extend
12
13
14
15
24   $Quota
25   $ObjId
26   $Reparse
27   $RmMetadata
28   $Repair
29   $Deleted
30   $TxfLog
31   $Txf
32   $Tops
33   $TxfLog.blf
34   $TxfLogContainer00000000000000000001
35   $TxfLogContainer00000000000000000002
36   System Volume Information
*/

UINT64 CNtfsMtfReader::EnumFiles(IMftReaderHandler* hander, PVOID Param)
{
	__super::EnumFiles(hander, Param);
	
	if (!m_MftBitmap)
		return 0;

	UINT64 result = 0;	
	CNtfsFile file(this);

	for(UINT64 i = 15; i < m_FileCount; i++)
	{

		if( !bitset(m_MftBitmap, i))
			continue;
		
		if (!ReadFileRecord(i, m_FileCanche))
			continue;

		if(m_FileCanche->Ntfs.Type != 'ELIF')
			continue;

		if((m_FileCanche->Flags & 0x1) == 0)
			continue;

		//file.Clear();
		file.LoadAttributes(i, m_FileCanche, true);
		if(file.GetFileData()->NameLength == 0)    //没有文件名，是一个纯数据文件
			continue;

		result++;

		if(!Callback(i, file.GetFileData()))
		{
			break;
		}
	}

	ClearCache();

	return result;
}

UINT64 CNtfsMtfReader::EnumDeleteFiles(IMftDeleteReaderHandler* hander, PVOID Param)
{
	__super::EnumDeleteFiles(hander, Param);

	if (!m_MftBitmap)
		return 0;

	UINT64 result = 0;
	CNtfsFile file(this);

	GetBitmapFile();        //获取簇位图文件

	FILE_DATA_STREAM datastream = { 0 };
	UINT maxCount = 1024 * 100;
	datastream.Lcns = (PUINT64)malloc(maxCount * sizeof(UINT64));

	for (UINT64 i = 16; i < m_FileCount; i++)     //第二次循环处理已删除文件
	{
		if (!ReadFileRecord(i, m_FileCanche))
			continue;

		if (m_FileCanche->Ntfs.Type != 'ELIF')
			continue;

		if ((m_FileCanche->Flags & 0x2) != 0)    //目录
		{
			file.LoadAttributes(i, m_FileCanche, true);
		}
		else
		{
			if ((m_FileCanche->Flags & 0x1) != 0)         //没有被删除文件
				continue;

			file.LoadAttributes(i, m_FileCanche, false);

			if (file.GetFileData()->NameLength == 0)    //没有文件名，是一个纯数据文件
				continue;

			//for (int m = 0; m < file.GetDataStreamCount(); m++)    //只取默认数据流
			CNtfsFileAttribute* attr = file.GetDataStream(0);
			if (attr)
			{
				if (!attr->IsNonresident())          //
				{
					UINT64 lcn = GetFileLcn(i);
					UINT offset = ((i * m_BytesPerFileRecord) % (m_BytesPerCluster));  //
					datastream.Offset = offset + attr->GetOffset() + attr->GetValueOffset();
					datastream.LcnCount = 1;						
					datastream.Lcns[0] = lcn;
					datastream.Size = attr->GetSize();
				}
				else
				{
					datastream.Offset = 0;
					datastream.LcnCount = 0;
					datastream.Size = attr->GetSize();

					for (int n = 0; n < attr->GetBlockCount(); n++)
					{

						PLCN_BLOCK block = attr->GetLcnBlock(n);
						if (datastream.LcnCount + block->nCount > maxCount)
						{
							maxCount = datastream.LcnCount + block->nCount;
							datastream.Lcns = (PUINT64)realloc(datastream.Lcns, maxCount * sizeof(UINT64));
						}

						for (UINT x = 0; x < block->nCount; x++)
						{
							if (IsClusterUsed(block->startLcn + x))
							{
								datastream.Lcns[datastream.LcnCount] = (block->startLcn + x) | 0x8000000000000000;
							}
							else
								datastream.Lcns[datastream.LcnCount] = (block->startLcn + x);
							datastream.LcnCount++;
						}
					}
				}
			}
			else
			{
				continue;
			}

		}
		result++;


		if (!DelCallback(i, file.GetFileData(), &datastream))
		{
			break;
		}

	}

	ClearCache();

	if (datastream.Lcns)
		free(datastream.Lcns);

	return result;

}

BOOL CNtfsMtfReader::GetFileStats(PUINT64 FileCount, PUINT64 FolderCount, PUINT64 DeletedFileTracks, PUINT64 DeleteFileCount)
{
	__super::GetFileStats(FileCount, FolderCount, DeletedFileTracks, DeleteFileCount);
	
	if (!m_MftBitmap)
		return FALSE;

	for (UINT64 i = 15; i < m_FileCount; i++)
	{

		if (!bitset(m_MftBitmap, i))
		{
			if (DeletedFileTracks)
				(*DeletedFileTracks)++;
		}

		if (!ReadFileRecord(i, m_FileCanche))
			continue;

		if (m_FileCanche->Ntfs.Type != 'ELIF')
			continue;

		if ((m_FileCanche->Flags & 0x1) == 0)
		{
			if (DeletedFileTracks)
				(*DeletedFileTracks)++;
			if (DeleteFileCount && (m_FileCanche->Flags & 0x2) == 0)
			{
				CNtfsFile file(this);
				file.LoadAttributes(i, m_FileCanche, false);
				if (file.GetFileData()->DataSize)
					(*DeleteFileCount)++;
			}
			continue;
		}

		if ((m_FileCanche->Flags & 0x2) != 0)    //目录
		{
			if(FolderCount)
				(*FolderCount)++;
		}
		else
			if (FileCount)
				(*FileCount)++;

	}
	return TRUE;
}

VOID CNtfsMtfReader::FixupUpdateSequenceArray(PNTFS_FILE_RECORD_HEADER file)
{
	if(file->Ntfs.UsaCount == 0)
		return;

	//DebugOutput(L"UsaOffset=%d UsaCount=%d", file->Ntfs.UsaOffset, file->Ntfs.UsaCount);

	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);

	for (UINT i = 1; i < file->Ntfs.UsaCount; i++)
	{
		if (PUCHAR(sector + 255) - PUCHAR(file) > m_BytesPerFileRecord)
			break;

		sector[255] = usa[i];
		sector += 256;
	}
}

CMftFile* CNtfsMtfReader::GetFile(UINT64 FileNumber, bool OnlyName)
{
	/*if (!bitset(m_MftBitmap, FileNumber))
		return nullptr;*/

	CNtfsFile* result = nullptr;
	PNTFS_FILE_RECORD_HEADER file = (PNTFS_FILE_RECORD_HEADER)new PUCHAR[m_BytesPerFileRecord];
	do
	{
		if (!ReadFileRecord(FileNumber, file, false))
			break;
		if (file->Ntfs.Type != 'ELIF')
			break;;
		/*if ((file->Flags & 0x1) == 0)
			break;*/
		result = new CNtfsFile(this);
		result->LoadAttributes(FileNumber, file, OnlyName);
	} while (false);


	delete file;

	return (CMftFile*)result;
}

BOOL CNtfsMtfReader::ReadFileData(CNtfsFile* file, UINT64 vcn, UINT count, PVOID buffer)
{
	
	UINT n = 0;
	UINT64 lcn = file->Vcn2Lcn(vcn, &n);

	while (lcn != 0 && n != 0)
	{
		UINT nRead;
		if (n > count)
			nRead = count;
		else
			nRead = n;

		if (!ReadCluster(lcn, nRead, buffer))
			return FALSE;
		buffer = (PUCHAR)buffer + nRead * m_BytesPerCluster;

		if (count <= nRead)
			break;

		count -= nRead;
		lcn = file->Vcn2Lcn(vcn + nRead, &n);
	}
	
	return TRUE;
}

BOOL CNtfsMtfReader::CacheFileData(CNtfsFile* file, UINT64 vcn, UINT count)
{
	if (!m_FatCache.Buffer)
	{
		m_FatCache.Buffer = new BYTE[count * m_BytesPerCluster];
	}
	else if(m_FatCache.Count < count)
	{
		delete m_FatCache.Buffer;
		m_FatCache.Buffer = new BYTE[count * m_BytesPerCluster];
	}

	if (!ReadFileData(file, vcn, count, m_FatCache.Buffer))
	{
		ClearCache();
		return FALSE;
	}

	m_FatCache.StartVcn = vcn;
	m_FatCache.Count = count;
	return TRUE;
}

UINT CNtfsMtfReader::CanReadFromCache(UINT64 lcn, UINT count)
{
	if (m_FatCache.Buffer && lcn >= m_FatCache.StartVcn && lcn + count <= m_FatCache.StartVcn + m_FatCache.Count)
	{
		return (UINT)(lcn - m_FatCache.StartVcn + 1);
	}

	return 0;
}

BOOL CNtfsMtfReader::bitset(PUCHAR bitmap, UINT64 i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

BOOL CNtfsMtfReader::ReadFileRecord(UINT64 index, PNTFS_FILE_RECORD_HEADER file, bool cache)
{
	UINT cluster = m_BytesPerFileRecord /m_BytesPerCluster + 1;
	UINT MAX_CACH_CLUMSTER_COUNT = (1024 * 1024 * 6) / m_BytesPerCluster;

	BOOL result = FALSE;

	PUCHAR p = nullptr;// new UCHAR[m_BytesPerFileRecord];
	UINT64 vcn = (UINT64)(index * m_BytesPerFileRecord)/m_BytesPerCluster;
	UINT byteOffset = index * m_BytesPerFileRecord - vcn * m_BytesPerCluster;

	//UINT n = 0;
	//UINT64 lcn = m_MftFile.Vcn2Lcn(vcn, &n);

	UINT cachVcn = CanReadFromCache(vcn, cluster);
	if (cachVcn)
	{
		p = m_FatCache.Buffer + (cachVcn - 1) * m_BytesPerCluster;
		MoveMemory(file, p + byteOffset, m_BytesPerFileRecord);
		result = TRUE;
	}
	else if (cache)
	{
		if (CacheFileData(&m_MftFile, vcn, MAX_CACH_CLUMSTER_COUNT))
		{
			p = m_FatCache.Buffer;
			MoveMemory(file, p + byteOffset, m_BytesPerFileRecord);
			result = TRUE;
		}
	}
	else
	{
		p = new UCHAR[cluster * m_BytesPerCluster];

		if (ReadFileData(&m_MftFile, vcn, cluster, p))
		{
			MoveMemory(file, p + byteOffset, m_BytesPerFileRecord);
			result = TRUE;
		}
		delete[] p;
	}

	if (result)
		FixupUpdateSequenceArray(file);
	else 
	{
		NTFS_FILE_RECORD_INPUT_BUFFER mftRecordInput = {0};
		DWORD dwCB;
		mftRecordInput.FileReferenceNumber.QuadPart = index;
		
		p = new UCHAR[sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) + m_BytesPerFileRecord];

		if (DeviceIoControl(m_Handle, FSCTL_GET_NTFS_FILE_RECORD, &mftRecordInput, sizeof(mftRecordInput), p, sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) + m_BytesPerFileRecord, &dwCB, NULL)
			&& PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileReferenceNumber.QuadPart == index)
		{
			memcpy(file, PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileRecordBuffer, m_BytesPerFileRecord);
			result = TRUE;
		}

		delete[] p;
	}


	return result;
}


UINT64 CNtfsMtfReader::ReadFileAttributeData(CNtfsFile* File, CNtfsFileAttribute* attr, PUCHAR buffer, UINT Size)
{
	if (!attr)
		return 0;

	if (attr->IsNonresident() == FALSE)
	{
		if (Size > attr->GetSize())
			Size = (UINT)attr->GetSize();
		MoveMemory(buffer, (PUCHAR)Padd(File->GetFileRecord(), attr->GetValueOffset() + attr->GetOffset()), Size);
		return Size;
	}
	else
	{
		UINT count = Size / m_BytesPerCluster;
		if (count*m_BytesPerCluster < Size)
			count++;

		//UINT nReaded = 0;
		for (int i = 0; i < attr->GetBlockCount(); i++)
		{
			UINT nCopy;

			if (attr->GetLcnBlock(i)->nCount > count)
				nCopy = count;
			else
				nCopy = attr->GetLcnBlock(i)->nCount;

			ReadCluster(attr->GetLcnBlock(i)->startLcn, nCopy, buffer);
			buffer += nCopy * m_BytesPerCluster;

			count -= nCopy;
			if(count == 0)
				break;
		}

		return Size - count * m_BytesPerCluster;
	}
}

bool CNtfsMtfReader::GetBitmapFile()
{
	if (!ReadFileRecord(6, m_FileCanche, true))
		return false;
	m_ClusterBitmap.LoadAttributes(6, m_FileCanche, false);
	CactchClusterBitmap(0);
	return true;
}

bool CNtfsMtfReader::IsClusterUsed(UINT64 ClusterNumber)
{
	UCHAR Result = 0;
	UINT64 byteOffset = ClusterNumber / 8;

	UINT64 vcn = byteOffset / m_BytesPerCluster;

	if (m_ClusterBitmapCache.Buffer && m_ClusterBitmapCache.StartVcn <= vcn && m_ClusterBitmapCache.StartVcn + m_ClusterBitmapCache.Count > vcn)
	{
		Result = *(PUCHAR)(m_ClusterBitmapCache.Buffer + (vcn - m_ClusterBitmapCache.StartVcn) * m_BytesPerCluster + byteOffset % m_BytesPerCluster);
	}
	else
	{
		PUCHAR pFAT = CactchClusterBitmap(vcn);
		if (pFAT)
			Result = *(PUCHAR)(pFAT + byteOffset % m_BytesPerCluster);
	}
	return (Result & (1 << ClusterNumber % 8))>0;
}

PUCHAR CNtfsMtfReader::CactchClusterBitmap(UINT64 vcn)
{
	if (m_ClusterBitmapCache.Buffer)
	{
		delete[] m_ClusterBitmapCache.Buffer;
		m_ClusterBitmapCache.Buffer = nullptr;
	}

	UINT count = (1024 * 1024 * 9) / m_BytesPerCluster + 1;


	m_ClusterBitmapCache.Buffer = new BYTE[count * m_BytesPerCluster];

	if(!ReadFileData(&m_ClusterBitmap, vcn, count, m_ClusterBitmapCache.Buffer))
	{
		delete[] m_ClusterBitmapCache.Buffer;
		m_ClusterBitmapCache.Buffer = nullptr;
	}

	m_ClusterBitmapCache.StartVcn = vcn;
	m_ClusterBitmapCache.Count = count;
	return m_ClusterBitmapCache.Buffer;
}

void CNtfsMtfReader::ClearCache()
{
	if (m_FatCache.Buffer)
	{
		delete[] m_FatCache.Buffer;
	}
	ZeroMemory(&m_FatCache, sizeof(m_FatCache));
	if (m_ClusterBitmapCache.Buffer)
		delete[] m_ClusterBitmapCache.Buffer;
	ZeroMemory(&m_ClusterBitmapCache, sizeof(m_ClusterBitmapCache));
}

UINT64 CNtfsMtfReader::GetFileLcn(UINT64 FileReferenceNumber)
{
	UINT64 vcn = UINT64(FileReferenceNumber) * m_BytesPerFileRecord / m_BytesPerCluster;
	UINT64 lcn = m_MftFile.Vcn2Lcn(vcn, nullptr);
	return lcn;
}

void CNtfsMtfReader::ZeroMember()
{
	__super::ZeroMember();
	
	//m_MftRecord = NULL;
	m_MftBitmap = NULL;
	//m_MftData = NULL;
	m_FileCanche = NULL;
	m_FileCount = 0;
	m_SectorsPerCluster = 0;
	m_BytesPerFileRecord = 0;
	//m_BytesPerClusters = 0;

	ZeroMemory(&m_FatCache, sizeof(m_FatCache));	
	ZeroMemory(&m_ClusterBitmapCache, sizeof(m_ClusterBitmapCache));
}

bool CNtfsMtfReader::Init()
{
	ClearCache();

	if (!__super::Init())
		return false;

	bool result = false;
	PNTFS_BPB pBpb = (PNTFS_BPB)new UCHAR[m_BytesPerSector];

	do{
		if(!ReadSector(0, 1, pBpb))
			break;
		
		m_SectorsPerCluster = ((PNTFS_BPB)pBpb)->SectorsPerCluster;
		m_BytesPerCluster = m_BytesPerSector * m_SectorsPerCluster;

		m_TotalCluster = pBpb->TotalSectors / pBpb->SectorsPerCluster;

		m_BytesPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord < 0x80
			? ((PNTFS_BPB)pBpb)->ClustersPerFileRecord * ((PNTFS_BPB)pBpb)->SectorsPerCluster
			* ((PNTFS_BPB)pBpb)->BytesPerSector : 1 << (0x100 - ((PNTFS_BPB)pBpb)->ClustersPerFileRecord);

		UINT t = m_BytesPerFileRecord > m_BytesPerSector ? m_BytesPerFileRecord : m_BytesPerSector;
		if (!m_FileCanche)
		{
			m_FileCanche = PNTFS_FILE_RECORD_HEADER(new UCHAR[t]);
		}

		UINT n = (m_BytesPerFileRecord) / (m_BytesPerSector);
		if(!ReadSector((((PNTFS_BPB)pBpb)->MftStartLcn)*(m_SectorsPerCluster), n == 0 ? 1 : n, m_FileCanche))
			break;

		FixupUpdateSequenceArray(m_FileCanche);

		m_MftFile.LoadAttributes(0, m_FileCanche, false);

		if (!m_MftFile.GetBitmapAttribute())
			break;

		if (!m_MftBitmap)
			delete[] m_MftBitmap;

		m_MftBitmap = new UCHAR[(UINT)m_MftFile.GetBitmapAttribute()->GetAllocSize()];

		ReadFileAttributeData(&m_MftFile, m_MftFile.GetBitmapAttribute(), m_MftBitmap, m_MftFile.GetBitmapAttribute()->GetAllocSize());

		if (m_MftFile.GetDataStream() == nullptr)
			break;

		m_FileCount = m_MftFile.GetFileData()->DataSize/m_BytesPerFileRecord; //MTF的总项数

		result = true;
	} while (FALSE);


#ifdef _DEBUG
	DebugOutput(L"m_BytesPerSector =%d\nBytesPerFileRecord = %d\nSectorsPerCluster = %d\nTotalCluster = %lld\nFileCount = %lld\n", 
		m_BytesPerSector, m_BytesPerFileRecord, m_SectorsPerCluster, m_TotalCluster, m_FileCount);

	/*FILE *stream = fopen("e:\\abc.dat", "w");
	if (stream)
	{
		fwrite(pBpb, m_BytesPerSector, 1, stream);
		fclose(stream);
	}*/

	for (int j = 0; j < m_MftFile.GetDataStreamCount(); j++)
	{
		for (int i = 0; i < m_MftFile.GetDataStream(j)->GetBlockCount(); i++)
		{
			DebugOutput(L"MFT Data strart = %lld count = %d\n", m_MftFile.GetDataStream(j)->GetLcnBlock(i)->startLcn, m_MftFile.GetDataStream(j)->GetLcnBlock(i)->startLcn);
		}
	}
#endif // _DEBUG

	delete[] pBpb;
	
	return result;
}


BOOL CNtfsMtfReader::GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo)
{
	CMftFile* file = GetFile(ReferenceNumber);
	if (file)
	{
		MoveMemory(aFileInfo, file->GetFileData(), sizeof(MFT_FILE_DATA));
		delete file;
		return TRUE;
	}
	else
		return FALSE;
}

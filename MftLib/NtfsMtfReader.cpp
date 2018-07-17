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
{
	ZeroMember();
	m_Inited = Init();
}


CNtfsMtfReader::~CNtfsMtfReader(void)
{
	if(m_MftRecord)
		delete m_MftRecord;
	m_MftRecord = NULL;
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
	//MFT_FILE_DATA tmp;
	//CLdArray<UINT64> excludedir;
	//excludedir.Add(8);
	//excludedir.Add(11);

	for(UINT64 i = 16; i < m_FileCount; i++)
	{

		if( !bitset(m_MftBitmap, i))
			continue;
		
		if (!ReadFileRecord(i, m_FileCanche))
			continue;

		if(m_FileCanche->Ntfs.Type != 'ELIF')
			continue;

		if((m_FileCanche->Flags & 0x1) == 0)
			continue;

		file.Clear();
		file.LoadAttributes(i, m_FileCanche, true);
		if(file.GetFileData()->NameLength == 0)    //没有文件名，是一个纯数据文件
			continue;

		//if (i < 100)
		//{        //排除类似$MFT系统文件
		//	if (excludedir.IndexOf(file.GetFileData()->DirectoryFileReferenceNumber) >= 0)
		//	{
		//		if (m_FileCanche->Flags & 0x2)
		//			excludedir.Add(i);
		//		continue;
		//	}
		//}
		result++;
		//FileAttribute2Info(&tmp, name);
		//tmp.FileReferenceNumber = i;
		if(!Callback(i, file.GetFileData()))
		{
			break;
		}
	}

	ClearCache();

	return result;
}

VOID CNtfsMtfReader::FixupUpdateSequenceArray(PNTFS_FILE_RECORD_HEADER file)
{
	if(file->Ntfs.UsaCount == 0)
		return;

	ULONG i = 0;
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);
	ULONG X = m_BytesPerSector / sizeof(USHORT), index;
	index = X -1;
	for (i = 1; i < file->Ntfs.UsaCount; i++)
	{
		if (sector[index] != usa[0])
		{
			return;
		}
		sector[index] = usa[i];
		index += X;
	}
}

CMftFile* CNtfsMtfReader::GetFile(UINT64 FileNumber, bool OnlyName)
{
	CNtfsFile* result = nullptr;
	PNTFS_FILE_RECORD_HEADER file = (PNTFS_FILE_RECORD_HEADER)new PUCHAR[m_BytesPerFileRecord];
	ZeroMemory(file, m_BytesPerFileRecord);
	do 
	{
		if (!ReadFileRecord(FileNumber, file, false))
			break;
		result = new CNtfsFile(this);
		result->LoadAttributes(FileNumber, file, OnlyName);
	} while (false);

	delete file;

	return (CMftFile*)result;
}

BOOL CNtfsMtfReader::ReadFileData(CNtfsFile* file, UINT64 vcn, ULONG count, PVOID buffer, bool cache)
{
	
#define MAX_CACH_CLUMSTER_COUNT 40

	UINT64 lcn;
	UINT n = 0;
	lcn = file->Vcn2Lcn(vcn, &n);
	UINT cachVcn = CanReadFromCache(lcn, count);

	if (!cachVcn && cache)
	{
		if (CacheFileData(file, lcn, n > MAX_CACH_CLUMSTER_COUNT ? MAX_CACH_CLUMSTER_COUNT : n))
			cachVcn = 1;
	}
	if(cachVcn)
		MoveMemory(buffer, m_Cache.Buffer + (cachVcn - 1) * m_BytesPerSector * m_SectorsPerCluster, count * m_BytesPerSector * m_SectorsPerCluster);
	else
	{
		while (true)
		{
			UINT nRead;
			if (n > count)
				nRead = count;
			else
				nRead = n;

			if (!ReadLCN(lcn, nRead, buffer))
				break;
			buffer = (PUCHAR)buffer + nRead * m_BytesPerSector * m_SectorsPerCluster;

			count -= nRead;

			if(count==0)
				break;

			lcn = file->Vcn2Lcn(vcn + nRead, &n);
		}
	}
	
	return TRUE;
}

BOOL CNtfsMtfReader::CacheFileData(CNtfsFile* file, UINT64 lcn, UINT count)
{
	if (!m_Cache.Buffer)
	{
		m_Cache.Buffer = new BYTE[count * m_SectorsPerCluster * m_BytesPerSector];
	}
	else if(m_Cache.Count < count)
	{
		delete m_Cache.Buffer;
		m_Cache.Buffer = new BYTE[count * m_SectorsPerCluster * m_BytesPerSector];
	}

	if (!ReadLCN(lcn, count, m_Cache.Buffer))
	{
		ClearCache();
		return FALSE;
	}

	m_Cache.StartLcn = lcn;
	m_Cache.Count = count;
	return TRUE;
}

UINT CNtfsMtfReader::CanReadFromCache(UINT64 lcn, UINT count)
{
	if (m_Cache.Buffer && lcn >= m_Cache.StartLcn && lcn + count <= m_Cache.StartLcn + m_Cache.Count)
	{
		return (UINT)(lcn - m_Cache.StartLcn + 1);
	}

	return 0;
}

BOOL CNtfsMtfReader::ReadLCN(UINT64 lcn, ULONG count, PVOID buffer)
{
	return ReadSector(lcn * m_SectorsPerCluster,count * m_SectorsPerCluster, buffer);
}

BOOL CNtfsMtfReader::bitset(PUCHAR bitmap, UINT64 i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

BOOL CNtfsMtfReader::ReadFileRecord(UINT64 index, PNTFS_FILE_RECORD_HEADER file, bool cache)
{
	ULONG clusters = m_ClustersPerFileRecord;
	BOOL result = FALSE;

	if (clusters > 0x80)
		clusters = 1;

	PUCHAR p = new UCHAR[m_BytesPerSector* m_SectorsPerCluster * clusters];
	UINT64 vcn = UINT64(index) * m_BytesPerFileRecord/m_BytesPerSector/m_SectorsPerCluster;


	if(ReadFileData(&m_MftFile, vcn, clusters, p, cache))
	{
		LONG m = (m_SectorsPerCluster * m_BytesPerSector / m_BytesPerFileRecord) - 1;
		ULONG n = m > 0 ? (index & m) : 0;
		memcpy(file, p + n * m_BytesPerFileRecord, m_BytesPerFileRecord);

		FixupUpdateSequenceArray(file);
		result = TRUE;
	}else
	{
		NTFS_FILE_RECORD_INPUT_BUFFER mftRecordInput = {0};
		DWORD dwCB;
		mftRecordInput.FileReferenceNumber.QuadPart = index;
		if (DeviceIoControl(m_Handle, FSCTL_GET_NTFS_FILE_RECORD, &mftRecordInput, sizeof(mftRecordInput), p, sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) + m_BytesPerFileRecord, &dwCB, NULL)
			&& PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileReferenceNumber.QuadPart == index)
		{
			memcpy(file, PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileRecordBuffer, m_BytesPerFileRecord);
			result = TRUE;
		}
	}

	delete [] p;
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
		UINT count = Size / m_BytesPerSector / m_SectorsPerCluster;
		if (count*m_BytesPerSector * m_SectorsPerCluster < Size)
			count++;

		//UINT nReaded = 0;
		for (UINT i = 0; i < attr->GetBlockCount(); i++)
		{
			UINT nCopy;

			if (attr->GetLcnBlock(i)->nCount > count)
				nCopy = count;
			else
				nCopy = attr->GetLcnBlock(i)->nCount;

			ReadLCN(attr->GetLcnBlock(i)->startLcn, nCopy, buffer);
			buffer += nCopy * m_BytesPerSector * m_SectorsPerCluster;

			count -= nCopy;
			if(count == 0)
				break;
		}

		return Size - count * m_BytesPerSector * m_SectorsPerCluster;
	}
}

void CNtfsMtfReader::ClearCache()
{
	if (m_Cache.Buffer)
	{
		delete[] m_Cache.Buffer;
		m_Cache.Buffer = NULL;
	}
	m_Cache.StartLcn = 0;
	m_Cache.Count = 0;
}

void CNtfsMtfReader::ZeroMember()
{
	__super::ZeroMember();
	
	m_MftRecord = NULL;
	m_MftBitmap = NULL;
	//m_MftData = NULL;
	m_FileCanche = NULL;
	m_FileCount = 0;
	m_SectorsPerCluster = 0;
	m_BytesPerFileRecord = 0;
	m_ClustersPerFileRecord = 0;
	m_BytesPerClusters = 0;

	ZeroMemory(&m_Cache, sizeof(m_Cache));	
}

bool CNtfsMtfReader::Init()
{
	if (!__super::Init())
		return false;

	bool result = false;
	PUCHAR pBpb = new UCHAR[m_BytesPerSector];

	do{
		if(!ReadSector(0, 1, pBpb))
			break;
		
		m_BytesPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord < 0x80
			? ((PNTFS_BPB)pBpb)->ClustersPerFileRecord * ((PNTFS_BPB)pBpb)->SectorsPerCluster
			* ((PNTFS_BPB)pBpb)->BytesPerSector: 1 << (0x100 - ((PNTFS_BPB)pBpb)->ClustersPerFileRecord);

		m_SectorsPerCluster = ((PNTFS_BPB)pBpb)->SectorsPerCluster;
		m_ClustersPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord;

		m_BytesPerClusters = m_BytesPerSector * m_SectorsPerCluster;

		if(!m_MftRecord)
			m_MftRecord = PNTFS_FILE_RECORD_HEADER(new UCHAR[m_BytesPerFileRecord]);
		ZeroMemory(m_MftRecord, m_BytesPerFileRecord);
		if(!ReadSector((((PNTFS_BPB)pBpb)->MftStartLcn)*(m_SectorsPerCluster), (m_BytesPerFileRecord)/(m_BytesPerSector), m_MftRecord))
			break;
		FixupUpdateSequenceArray(m_MftRecord);

		m_MftFile.LoadAttributes(0, m_MftRecord, false);
		if (m_MftFile.GetBitmapAttribute())
		{
			if (!m_MftBitmap)
				delete[] m_MftBitmap;
			m_MftBitmap = new UCHAR[(ULONG)m_MftFile.GetBitmapAttribute()->GetAllocSize()];

			ReadFileAttributeData(&m_MftFile, m_MftFile.GetBitmapAttribute(), m_MftBitmap, m_MftFile.GetBitmapAttribute()->GetAllocSize());
		}

		if(!m_FileCanche)
			m_FileCanche = PNTFS_FILE_RECORD_HEADER(new UCHAR[m_BytesPerFileRecord]);

		m_FileCount = m_MftFile.GetDataStream()->GetSize()/m_BytesPerFileRecord; //MTF的总项数

		result = true;
	} while (FALSE);

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

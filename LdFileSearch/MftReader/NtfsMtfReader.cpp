#include "stdafx.h"
#include "NtfsMtfReader.h"

#define MAX_CACHE_SECTORS  4000

template <class T1, class T2> inline T1* Padd(T1* p, T2 n)
{
	return (T1*)((char *)p + n);
}

CNtfsMtfReader::CNtfsMtfReader()
{
	ZeroMember();
}


CNtfsMtfReader::~CNtfsMtfReader(void)
{
	if(m_Mft)
		delete m_Mft;
	m_Mft = NULL;
	if(m_MftBitmap)
		delete m_MftBitmap;
	m_MftBitmap = NULL;

	if(!m_File)
		delete m_File;
	m_File = NULL;

	if (m_cache_info.g_pb){
		delete [] m_cache_info.g_pb;
		m_cache_info.g_pb = NULL;
		m_cache_info.cache_lcn_begin = 0;
		m_cache_info.cache_lcn_count = 0;
		m_cache_info.cache_lcn_orl_begin = 0;
		m_cache_info.cache_lcn_total = 0;
	}

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

UINT64 CNtfsMtfReader::EnumFiles(PVOID Param)
{
	UINT64 result = 0;	
	if (!Init())
		return 0;

	for(UINT64 i = 0; i < m_FileCount; i++){
//		if (!bitset(m_MftBitmap, i)) //跳过被删除的文件
//			continue;
		PFILENAME_ATTRIBUTE name = ReadFileNameInfoEx(i);

		if(name){
			result++;

			if(!Callback(i, FileAttribute2Info(name), Param))
			{
				
				break;
			}
		}
	}
	
	return result;
}

VOID CNtfsMtfReader::FixupUpdateSequenceArray(PFILE_RECORD_HEADER file)
{
	if(file->Ntfs.UsaCount == 0)
		return;

	DWORD i = 0;
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);
	DWORD X = m_BpbData->BytesPerSector / sizeof(USHORT), index;
	index = X -1;
	for (i = 1; i < file->Ntfs.UsaCount; i++){
		if (sector[index] != usa[0]){
			return;
		}
		sector[index] = usa[i];
		index += X;
	}
}

PATTRIBUTE CNtfsMtfReader::EnumAttribute(PATTRIBUTE pAttrHeader, DWORD size, ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count /*= 0*/)
{
	PATTRIBUTE result = NULL, attr = NULL;
	DWORD tl = 0;
	int n_ctrl = 0;
	for (attr = pAttrHeader; tl < size;attr = Padd(attr, attr->Length))
	{
		if (attr->AttributeType == -1) break;
		if (attr->AttributeType == 0x0) break;
		if (attr->Length == 0) break;
		tl += attr->Length;

		if (attr->AttributeType == type)
		{
			if (n_attr_count != n_ctrl)
			{
				n_ctrl++;
				continue;
			}
			if (name == 0 && attr->NameLength == 0)
				return attr;
			if (name != 0 && _tcslen(name) == attr->NameLength && _wcsicmp(name,PWSTR(Padd(attr, attr->NameOffset))) == 0)
			{
				result = attr;
				break;
			}
		}
		else if (attr->AttributeType == AttributeAttributeList)
		{
			//printf("AttributeAttributeList %d\n", attr->Nonresident);
			if (!attr->Nonresident)
			{
				//printf("Attribute Flag=%d, Length=%d, Offset=%d\n", PRESIDENT_ATTRIBUTE(attr)->Flags, PRESIDENT_ATTRIBUTE(attr)->ValueLength, PRESIDENT_ATTRIBUTE(attr)->ValueOffset);
				result = EnumAttribute(Padd(attr, PRESIDENT_ATTRIBUTE(attr)->ValueOffset), PRESIDENT_ATTRIBUTE(attr)->ValueLength, type, name, n_attr_count);
			}
			else
			{
//				printf("Nonresident Attribute DataSize=%lld, HighVcn=%lld, LowVcn=%lld, RunArrayOffset=%lld\n", 
//					PNONRESIDENT_ATTRIBUTE(attr)->DataSize, 
//					PNONRESIDENT_ATTRIBUTE(attr)->HighVcn, 
//					PNONRESIDENT_ATTRIBUTE(attr)->LowVcn,
//					PNONRESIDENT_ATTRIBUTE(attr)->RunArrayOffset);
				PUCHAR p = new UCHAR[m_BpbData->BytesPerClusters];
				ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE(attr), 0, 1, p, true);
				result = EnumAttribute(PATTRIBUTE(p), PNONRESIDENT_ATTRIBUTE(attr)->DataSize, type, name, n_attr_count);
				delete [] p;
			}
			if(result)
				break;
		}
	}
	return result;
}

PATTRIBUTE CNtfsMtfReader::FindAttribute(PFILE_RECORD_HEADER file,ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count /*= 0*/)
{
	PATTRIBUTE result = NULL, attr = NULL;

	result = EnumAttribute(PATTRIBUTE(Padd(file, file->AttributesOffset)), file->BytesInUse, type, name, n_attr_count);

	return result;
}

UINT64 CNtfsMtfReader::AttributeLengthAllocated(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize;
}

BOOL CNtfsMtfReader::ReadAttribute(PATTRIBUTE attr, PVOID buffer, DWORD size)
{
	PRESIDENT_ATTRIBUTE rattr = NULL;
	PNONRESIDENT_ATTRIBUTE nattr = NULL;

	if (attr->Nonresident == FALSE)
	{
		rattr = PRESIDENT_ATTRIBUTE(attr);
		if(size == 0)
			size = rattr->ValueLength;
		else if(size > rattr->ValueLength)
			size = rattr->ValueLength;
		memcpy(buffer, Padd(rattr, rattr->ValueOffset), size);
		return TRUE;
	}
	else
	{
		nattr = PNONRESIDENT_ATTRIBUTE(attr);
		if(size != 0)
			size = size / m_BpbData->BytesPerClusters;
		else
			size = DWORD(nattr->HighVcn) + 1;
		return ReadExternalAttribute(nattr, 0, size, buffer);
	}
}

BOOL CNtfsMtfReader::ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr,UINT64 vcn, DWORD count, PVOID buffer, bool b_allow_cache /*= false*/)
{
	DWORD readcount, left;
	PUCHAR bytes = (PUCHAR)buffer;
	BOOL result = FALSE;
	left = count;
	
	UINT64 lcn, runcount;
	for(left = count; left > 0; left -= readcount)
	{
		if(!FindRun(attr, vcn, &lcn, &runcount))
			break;

		if (NeedCache(lcn) && b_allow_cache)
		{
			NewCache(lcn, runcount);
		}
		readcount = DWORD(min(runcount, left));
		DWORD n = readcount * m_BpbData->BytesPerClusters;

		if(lcn == 0)
			memset(bytes, 0, n);
		else
		{
			if (CanReadFromCache(lcn))
			{
				memcpy(bytes, m_cache_info.g_pb + ((lcn - m_cache_info.cache_lcn_begin) * m_BpbData->BytesPerClusters), 
					readcount * m_BpbData->BytesPerClusters);
				result = readcount * m_BpbData->BytesPerClusters;
			}
			else
			{
				result = ReadLCN(lcn, readcount, bytes);
				if(!result){
					break;
				}
			}
		}          
		vcn += readcount;
		bytes += n;
	}
	
	return result;
}
/*
PRUN_LINKE CNtfsMtfReader::BuildRun(PNONRESIDENT_ATTRIBUTE attr)
{
	PUCHAR run = NULL;
	*lcn = 0;
	UINT64 base = attr->LowVcn & 0x0000FFFFFFFFFFFF, Lcn = 0;

	if (vcn < base || vcn >(attr->HighVcn & 0x0000FFFFFFFFFFFF))
		return FALSE;

	for (run = PUCHAR(Padd(attr, attr->RunArrayOffset)); *run != 0; run += RunLength(run))
	{
		INT64 offset = RunLCN(run);
		Lcn += offset;
		INT64 size = RunCount(run);

		if (base <= vcn && vcn < base + size)
		{
			*lcn = offset == 0 ? 0 : vcn + Lcn - base; //offset + vcn - base;
			*count = size + base - vcn;
			return TRUE;
		}
		base += size;
	}
	return FALSE;
}
*/
BOOL CNtfsMtfReader::FindRun(PNONRESIDENT_ATTRIBUTE attr, UINT64 vcn, PULONGLONG lcn, PULONGLONG count)
{
	PUCHAR run = NULL;
	*lcn = 0;
	UINT64 base = attr->LowVcn & 0x0000FFFFFFFFFFFF, Lcn = 0;
//
	if (vcn < base || vcn > (attr->HighVcn & 0x0000FFFFFFFFFFFF))
		return FALSE;

	for(run = PUCHAR(Padd(attr, attr->RunArrayOffset));   *run != 0;  run += RunLength(run))
	{
		INT64 offset = RunLCN(run);
		Lcn += offset;
		INT64 size = RunCount(run);


		if (base <= vcn && vcn < base + size)
		{
			*lcn = offset == 0 ? 0 : vcn+Lcn-base; //offset + vcn - base;
			*count = size + base - vcn;
			return TRUE;
		}
		base += size;
	}
	return FALSE;
}

BOOL CNtfsMtfReader::NeedCache(UINT64 lcn)
{
	if (!m_cache_info.g_pb)
	{
		return true;
	}
	if (lcn >= m_cache_info.cache_lcn_begin && lcn < m_cache_info.cache_lcn_begin + m_cache_info.cache_lcn_count)
	{
		return false;
	}
	return true;

}

void CNtfsMtfReader::NewCache(UINT64 lcn, UINT64 lcn_count)
{
	if (lcn < m_cache_info.cache_lcn_begin)
	{
		return;
	}
	if (m_cache_info.g_pb)
	{
		delete [] m_cache_info.g_pb;
		m_cache_info.g_pb = NULL;
	}
	UINT64 new_len;
	if (!m_cache_info.cache_lcn_orl_begin)
	{
		new_len = MAX_CACHE_SECTORS >= lcn_count ? lcn_count : MAX_CACHE_SECTORS;
		m_cache_info.cache_lcn_orl_begin = lcn;
		m_cache_info.cache_lcn_total = lcn_count;
	}
	else if (m_cache_info.cache_lcn_orl_begin && lcn > m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total)
	{
		new_len = MAX_CACHE_SECTORS >= lcn_count ? lcn_count : MAX_CACHE_SECTORS;
		m_cache_info.cache_lcn_orl_begin = lcn;
		m_cache_info.cache_lcn_total = lcn_count;
	}
	else
	{
		new_len = MAX_CACHE_SECTORS >= m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total - lcn ? m_cache_info.cache_lcn_orl_begin + m_cache_info.cache_lcn_total - lcn : MAX_CACHE_SECTORS;
	}
	BYTE* p_tmp = new BYTE[(DWORD)(new_len * m_BpbData->BytesPerClusters)];
	if (p_tmp)
	{
		m_cache_info.g_pb = p_tmp;
		m_cache_info.cache_lcn_begin = lcn;
		m_cache_info.cache_lcn_count = new_len;
		ReadLCN(lcn, (DWORD)new_len, m_cache_info.g_pb);
	}
}

BOOL CNtfsMtfReader::CanReadFromCache(UINT64 lcn)
{
	if (m_cache_info.g_pb && lcn >= m_cache_info.cache_lcn_begin && lcn < m_cache_info.cache_lcn_begin + m_cache_info.cache_lcn_count)
	{
		return TRUE;
	}
	return FALSE;

}

BOOL CNtfsMtfReader::ReadLCN(UINT64 lcn, DWORD count, PVOID buffer)
{
	return ReadSector(lcn * m_BpbData->SectorsPerCluster,count * m_BpbData->SectorsPerCluster, buffer);
}

INT64 CNtfsMtfReader::RunLCN(PUCHAR run)
{
	
	LONG i = 0;
	UCHAR n1 = 0 , n2 = 0;
	INT64 lcn = 0;

	n1 = *run & 0xf;
	n2 = (*run >> 4) & 0xf;

	lcn = n2 == 0 ? 0 : UCHAR(run[n1 + n2]);

	for (i = n1 + n2 - 1; i > n1; i--)
		lcn = (lcn << 8) + run[i];
	
	UCHAR k= n2; //!!!
	if (((PUCHAR)&lcn)[k - 1] >= 0x80)
		while (k < 8){
			((PUCHAR)&lcn)[k] = 0xFF;
			k++;
		}

	return lcn;
}

INT64 CNtfsMtfReader::RunCount(PUCHAR run)
{
	UCHAR n = *run & 0xf;
	UINT64 count = 0;
	DWORD i;


	for (i = n; i > 0; i--)
		count = (count << 8) + run[i];

	return count;
}

DWORD CNtfsMtfReader::RunLength(PUCHAR run)
{
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
}

UINT64 CNtfsMtfReader::AttributeLength(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : DWORD(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
}

BOOL CNtfsMtfReader::bitset(PUCHAR bitmap, UINT64 i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

BOOL CNtfsMtfReader::ReadFileRecord(PFILE_RECORD_HEADER Mft, UINT64 index, PFILE_RECORD_HEADER file)
{
	DWORD clusters = m_BpbData->ClustersPerFileRecord;

	if (clusters > 0x80 || clusters == 0)
		clusters = 1;

	PUCHAR p = new UCHAR[m_BpbData->BytesPerClusters * clusters];
	UINT64 vcn = UINT64(index) * m_BpbData->BytesPerFileRecordSegment/m_BpbData->SectorsPerCluster/ m_BpbData->BytesPerSector;
	if(ReadVCN(Mft, AttributeData, vcn, clusters, p))
	{
		LONG m = (m_BpbData->BytesPerClusters/ m_BpbData->BytesPerFileRecordSegment) - 1;
		DWORD n = m > 0 ? (index & m) : 0;
		memcpy(file, p + n * m_BpbData->BytesPerFileRecordSegment, m_BpbData->BytesPerFileRecordSegment);
		FixupUpdateSequenceArray(file);
	}else
	{
		NTFS_FILE_RECORD_INPUT_BUFFER mftRecordInput = {0};
		DWORD dwCB;
		mftRecordInput.FileReferenceNumber.QuadPart = index;
		if (DeviceIoControl(m_Handle, FSCTL_GET_NTFS_FILE_RECORD, &mftRecordInput, sizeof(mftRecordInput), p, 
			sizeof(NTFS_FILE_RECORD_OUTPUT_BUFFER) + m_BpbData->BytesPerFileRecordSegment, &dwCB, NULL)
			&& PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileReferenceNumber.QuadPart == index)
		{
			memcpy(file, PNTFS_FILE_RECORD_OUTPUT_BUFFER(p)->FileRecordBuffer, m_BpbData->BytesPerFileRecordSegment);
		}
		else
		{
			delete [] p;
			return FALSE;
		}

	}

	delete [] p;
	return TRUE;
}

BOOL CNtfsMtfReader::ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,UINT64 vcn, DWORD count, PVOID buffer)
{
	PATTRIBUTE attrlist = NULL;
	PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

	if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
	{
		return FALSE;
	}
	return ReadExternalAttribute(attr, vcn, count, buffer, TRUE);
}

const PFILENAME_ATTRIBUTE CNtfsMtfReader::ReadMtfFileNameAttribute(UINT64 ReferenceNumber)
{
	return ReadFileNameInfoEx(ReferenceNumber);
}

PFILENAME_ATTRIBUTE CNtfsMtfReader::ReadFileNameInfoEx(UINT64 ReferenceNumber)
{
	ZeroMemory(m_File, m_BpbData->BytesPerFileRecordSegment);

	if(!ReadFileRecord(m_Mft, ReferenceNumber, m_File)){
		return NULL;
	}
	
	if (m_File->Ntfs.Type == 'ELIF'){
		if((m_File->Flags & 1) == 0)
		{
			//return NULL;
		}

		PVOID nameAttr = FindAttribute(m_File, AttributeFileName, 0);
		if(!nameAttr)
		{
#ifdef _DEBUG
			//printf("no name attribute %d ", ReferenceNumber);
#endif
			return NULL;
		}

		PFILENAME_ATTRIBUTE name = PFILENAME_ATTRIBUTE(Padd(nameAttr,PRESIDENT_ATTRIBUTE(nameAttr)->ValueOffset));

		int n_ctrl = 1;
		PFILENAME_ATTRIBUTE name_lenname = NULL;
		PATTRIBUTE attr_lenname = NULL;
		BYTE file_type = name->NameType;

		while (file_type == 2){ //只取长文件名
			attr_lenname = FindAttribute(m_File, AttributeFileName, 0, n_ctrl);
			n_ctrl++;
			if (attr_lenname == 0)
				break;
			name_lenname = PFILENAME_ATTRIBUTE(Padd(attr_lenname,PRESIDENT_ATTRIBUTE(attr_lenname)->ValueOffset));
			file_type = name_lenname->NameType;
		}
		if (name_lenname != NULL){ //如果没有长文件名，就取短文件名
			name = name_lenname;
		}
		if(m_File->Flags & 2)
			name->FileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
		else{
			PATTRIBUTE dataAttr = FindAttribute(m_File, AttributeData, NULL);
			if(dataAttr)
				name->DataSize = AttributeLength(dataAttr);
		}
		return name;
	}else
	{
		return NULL;
	}
}

void CNtfsMtfReader::ZeroMember()
{
	__super::ZeroMember();

	m_Mft = NULL;
	m_MftBitmap = NULL;
	m_MftData = NULL;
	m_File = NULL;
	m_FileCount = 0;
	m_BpbData = nullptr;
	ZeroMemory(&m_cache_info, sizeof(m_cache_info));	
}

bool CNtfsMtfReader::Init()
{
	m_Handle = m_Volume->OpenVolumeHandle();
	if (m_Handle == INVALID_HANDLE_VALUE)
		return false;
	m_BpbData = m_Volume->GetVolumeMftData();

	if(!m_Mft)
		m_Mft = PFILE_RECORD_HEADER(new UCHAR[m_BpbData->BytesPerFileRecordSegment]);
	ZeroMemory(m_Mft, m_BpbData->BytesPerFileRecordSegment);
	if(!ReadSector(m_BpbData->MftStartLcn*(m_BpbData->SectorsPerCluster), (m_BpbData->BytesPerFileRecordSegment)/(m_BpbData->BytesPerSector), m_Mft)){
		return 0;
	}
	FixupUpdateSequenceArray(m_Mft);
	PATTRIBUTE attrBitmap = FindAttribute(m_Mft, AttributeBitmap, 0);
	if (attrBitmap)
	{
		UINT64 allocSize = AttributeLengthAllocated(attrBitmap);
		if (!m_MftBitmap)
			delete[] m_MftBitmap;
		m_MftBitmap = new UCHAR[(DWORD)allocSize];
		if (!ReadAttribute(attrBitmap, m_MftBitmap)) {//$MFT元文件中的$Bitmap属性，此属性中标识了$MFT元文件中MFT项的使用状况
			return 0;
		}
	}

	if(!m_File)
		m_File = PFILE_RECORD_HEADER(new UCHAR[m_BpbData->BytesPerFileRecordSegment]);

	m_FileCount = AttributeLength(FindAttribute(m_Mft, AttributeData, 0))/ m_BpbData->BytesPerFileRecordSegment; //MTF的总项数
	if (m_cache_info.g_pb){
		delete [] m_cache_info.g_pb;
		m_cache_info.g_pb = NULL;
		m_cache_info.cache_lcn_begin = 0;
		m_cache_info.cache_lcn_count = 0;
		m_cache_info.cache_lcn_orl_begin = 0;
		m_cache_info.cache_lcn_total = 0;
	}

	return true;
}

PFILE_INFO CNtfsMtfReader::FileAttribute2Info(PFILENAME_ATTRIBUTE name)
{
	if (!name)
		return NULL;

	static PFILE_INFO aFileInfo = (PFILE_INFO) new UCHAR[sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR)];
	ZeroMemory(aFileInfo, sizeof(FILE_INFO) + MAX_PATH * sizeof(TCHAR));
	aFileInfo->AllocatedSize = name->AllocatedSize;
	aFileInfo->ChangeTime = name->ChangeTime;
	aFileInfo->CreationTime = name->CreationTime;
	aFileInfo->DataSize = name->DataSize;
	aFileInfo->DirectoryFileReferenceNumber = name->DirectoryFileReferenceNumber;
	aFileInfo->FileAttributes = name->FileAttributes;
	aFileInfo->LastAccessTime = name->LastAccessTime;
	aFileInfo->LastWriteTime = name->LastWriteTime;
	aFileInfo->NameLength = name->NameLength;
	memcpy(aFileInfo->Name, name->Name, name->NameLength * sizeof(TCHAR));
	//wcscpy_s(aFileInfo->Name,name->NameLength * sizeof(TCHAR), name->Name);
	return aFileInfo;
}
/*

const PFILE_INFO CNtfsMtfReader::GetFileInfo(UINT64 ReferenceNumber)
{
	return FileAttribute2Info(ReadMtfFileNameAttribute(ReferenceNumber));
}


USN CNtfsMtfReader::GetLastUsn()
{
	if(m_Handle == INVALID_HANDLE_VALUE)
		return 0;

	USN_JOURNAL_DATA ujd = {0};

	if(QueryUsnStatus(m_Handle, &ujd)){
		return ujd.NextUsn;
	}else{
		CreateUsnJournal(m_Handle);
		if(QueryUsnStatus(m_Handle, &ujd))
			return ujd.NextUsn;
	}

	return 0;
}

BOOL CNtfsMtfReader::QueryUsnStatus(HANDLE hVolume, PUSN_JOURNAL_DATA outStatus)
{
	DWORD br;
	return DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, outStatus, sizeof(USN_JOURNAL_DATA), &br, NULL);
}
*/
/*

USN CNtfsMtfReader::UpdateFiles(USN LastUsn, PVOID param)
{
	if(m_Handle == INVALID_HANDLE_VALUE)
		return 0;

	USN_JOURNAL_DATA qujd = {0};

	if(!QueryUsnStatus(m_Handle, &qujd))
		return 0;

	USN ret = 0;

	if (LastUsn >= qujd.FirstUsn && LastUsn < qujd.NextUsn){
		BYTE* buffer = new BYTE[USN_PAGE_SIZE];
		if (buffer){
			DWORD BytesReturned = 0;
			READ_USN_JOURNAL_DATA rujd = {LastUsn == 0 ? qujd.FirstUsn : LastUsn , 
				USN_REASON_CLOSE/*USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_FILE_CREATE | USN_REASON_RENAME_OLD_NAME0xffffffff/*to get all reason#1#, 
				0, 0, 0, qujd.UsnJournalID};
			memset(buffer, 0, USN_PAGE_SIZE);
			DWORD cb = 0;
			BOOL fOk = TRUE;
			while (fOk) {
				fOk = DeviceIoControl(m_Handle, FSCTL_READ_USN_JOURNAL, &rujd, sizeof(rujd), buffer, USN_PAGE_SIZE, &cb, NULL);
				if (!fOk || (cb <= sizeof(USN))) 
					break;

				rujd.StartUsn = * (USN*) buffer;

				USN_RECORD *pUsnRecord = (PUSN_RECORD) &buffer[sizeof(USN)];

				while ((PBYTE) pUsnRecord < (buffer + cb)){
					//ret = pUsnRecord->Usn;
					fOk = m_Holder->EnumUsnRecordCallback(pUsnRecord, param);
					if(!fOk)
						break;

					pUsnRecord = (PUSN_RECORD) 
						((PBYTE) pUsnRecord + pUsnRecord->RecordLength);
				}
			}
			if (::GetLastError() == ERROR_JOURNAL_ENTRY_DELETED){
				ret = 0;
			}
		}
		if (buffer){
			delete [] buffer;
		}

		ret =qujd.NextUsn;
	}


	return ret;

}
*/
/*

BOOL CNtfsMtfReader::CreateUsnJournal(HANDLE hVolume)
{
	CREATE_USN_JOURNAL_DATA cujd = {1024 * 1024 * 100, 1024 * 1024 * 20};
	DWORD br;
	return DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
}

BOOL CNtfsMtfReader::DeleteUsnJournal(HANDLE hVolume)
{
	USN_JOURNAL_DATA uujd = {0};
	if(!QueryUsnStatus(hVolume, &uujd))
		return FALSE;
	DWORD br;
	DELETE_USN_JOURNAL_DATA dujd = {uujd.UsnJournalID, USN_DELETE_FLAG_DELETE || USN_DELETE_FLAG_NOTIFY};
	return DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL, &dujd, sizeof(dujd), NULL, 0, &br, NULL);
}
*/

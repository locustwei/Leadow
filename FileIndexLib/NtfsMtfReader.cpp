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

	if(m_hVolumeBack != INVALID_HANDLE_VALUE)
		CloseHandle(m_hVolumeBack);
}

ULONGLONG CNtfsMtfReader::EnumFiles(PVOID Param)
{
	__super::EnumFiles(Param);
	
	ULONGLONG result = 0;	

	for(ULONGLONG i = 34; i < m_FileCount; i++){
		if (!bitset(m_MftBitmap, i)) //跳过被删除的文件
			continue;
		PFILENAME_ATTRIBUTE name = ReadFileNameInfoEx(i);

		if(name){
			result++;

			if(!Callback(i, FileAttribute2Info(name), Param))
				break;
		}
	}

#ifdef _DEBUG
	printf("%lld files found \n", result);
#endif // _DEBUG
	

	return result;
}

VOID CNtfsMtfReader::FixupUpdateSequenceArray(PFILE_RECORD_HEADER file)
{
	if(file->Ntfs.UsaCount == 0)
		return;

	ULONG i = 0;
	PUSHORT usa = PUSHORT(Padd(file, file->Ntfs.UsaOffset));
	PUSHORT sector = PUSHORT(file);
	ULONG X = m_BytesPerSector / sizeof(USHORT), index;
	index = X -1;
	for (i = 1; i < file->Ntfs.UsaCount; i++){
		if (sector[index] != usa[0]){
			return;
		}
		sector[index] = usa[i];
		index += X;
	}
}

PATTRIBUTE CNtfsMtfReader::FindAttribute(PFILE_RECORD_HEADER file,ATTRIBUTE_TYPE type, PWSTR name, int n_attr_count /*= 0*/)
{
	PATTRIBUTE attr = NULL;

	int n_ctrl = 0;
	for (attr = PATTRIBUTE(Padd(file, file->AttributesOffset));
		attr->AttributeType != -1;attr = Padd(attr, attr->Length))
	{
		if (attr->AttributeType == type)
		{
			if (n_attr_count != n_ctrl)
			{
				n_ctrl++;
				continue;
			}
			if (name == 0 && attr->NameLength == 0)
				return attr;
			if (name != 0 && wcslen(name) == attr->NameLength && _wcsicmp(name,
				PWSTR(Padd(attr, attr->NameOffset))) == 0)
				return attr;
		}
	}
	return NULL;
}

ULONGLONG CNtfsMtfReader::AttributeLengthAllocated(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : PNONRESIDENT_ATTRIBUTE(attr)->AllocatedSize;
}

BOOL CNtfsMtfReader::ReadAttribute(PATTRIBUTE attr, PVOID buffer, ULONG size)
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
			size = size / m_BytesPerSector / m_SectorsPerCluster;
		else
			size = ULONG(nattr->HighVcn) + 1;
		return ReadExternalAttribute(nattr, 0, size, buffer);
	}
}

BOOL CNtfsMtfReader::ReadExternalAttribute(PNONRESIDENT_ATTRIBUTE attr,ULONGLONG vcn, ULONG count, PVOID buffer, bool b_allow_cache /*= false*/)
{
	ULONG readcount, left;
	PUCHAR bytes = (PUCHAR)buffer;
	BOOL result = FALSE;
	left = count;
	
	ULONGLONG lcn, runcount;
	for(left = count; left > 0; left -= readcount)
	{
		if(!FindRun(attr, vcn, &lcn, &runcount))
			break;

		if (NeedCache(lcn) && b_allow_cache)
		{
			NewCache(lcn, runcount);
		}
		readcount = ULONG(min(runcount, left));
		ULONG n = readcount * m_BytesPerSector * m_SectorsPerCluster;

		if(lcn == 0)
			memset(bytes, 0, n);
		else
		{
			if (CanReadFromCache(lcn))
			{
				memcpy(bytes, m_cache_info.g_pb + ((lcn - m_cache_info.cache_lcn_begin) * m_SectorsPerCluster * m_BytesPerSector), readcount * m_SectorsPerCluster * m_BytesPerSector);
				result = readcount * m_SectorsPerCluster * m_BytesPerSector;
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
	ULONGLONG base = attr->LowVcn & 0x0000FFFFFFFFFFFF, Lcn = 0;

	PRUN_LINKE pNext = NULL, result = NULL;
	for(PUCHAR run = PUCHAR(Padd(attr, attr->RunArrayOffset));   *run != 0;  run += RunLength(run))
	{
		LONGLONG offset = RunLCN(run);
		Lcn += offset;
		LONGLONG size = RunCount(run);

		PRUN_LINKE pRun = new RUN_LINKE;
		pRun->lcn = Lcn + base;
		pRun->size = (ULONG)size;
		
		Lcn += size;

		pRun->next = NULL;
		if(pNext)
			pNext->next = pRun;
		if(!result)
			result = pRun;
		pNext = pRun;

	}
	return result;
}
*/
BOOL CNtfsMtfReader::FindRun(PNONRESIDENT_ATTRIBUTE attr, ULONGLONG vcn, PULONGLONG lcn, PULONGLONG count)
{
	PUCHAR run = NULL;
	*lcn = 0;
	ULONGLONG base = attr->LowVcn & 0x0000FFFFFFFFFFFF, Lcn = 0;

	if (vcn < base || vcn > (attr->HighVcn & 0x0000FFFFFFFFFFFF))
		return FALSE;

	for(run = PUCHAR(Padd(attr, attr->RunArrayOffset));   *run != 0;  run += RunLength(run))
	{
		LONGLONG offset = RunLCN(run);
		Lcn += offset;
		LONGLONG size = RunCount(run);
		
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

BOOL CNtfsMtfReader::NeedCache(ULONGLONG lcn)
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

void CNtfsMtfReader::NewCache(ULONGLONG lcn, ULONGLONG lcn_count)
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
	ULONGLONG new_len;
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
	BYTE* p_tmp = new (std::nothrow) BYTE[(ULONG)(new_len * m_SectorsPerCluster * m_BytesPerSector)];
	if (p_tmp)
	{
		m_cache_info.g_pb = p_tmp;
		m_cache_info.cache_lcn_begin = lcn;
		m_cache_info.cache_lcn_count = new_len;
		ReadLCN(lcn, (ULONG)new_len, m_cache_info.g_pb);
	}
}

BOOL CNtfsMtfReader::CanReadFromCache(ULONGLONG lcn)
{
	if (m_cache_info.g_pb && lcn >= m_cache_info.cache_lcn_begin && lcn < m_cache_info.cache_lcn_begin + m_cache_info.cache_lcn_count)
	{
		return TRUE;
	}
	return FALSE;

}

BOOL CNtfsMtfReader::ReadLCN(ULONGLONG lcn, ULONG count, PVOID buffer)
{
	return ReadSector(lcn * m_SectorsPerCluster,count * m_SectorsPerCluster, buffer);
}

LONGLONG CNtfsMtfReader::RunLCN(PUCHAR run)
{
	
	LONG i = 0;
	UCHAR n1 = 0 , n2 = 0;
	LONGLONG lcn = 0;

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

LONGLONG CNtfsMtfReader::RunCount(PUCHAR run)
{
	UCHAR n = *run & 0xf;
	ULONGLONG count = 0;
	ULONG i;


	for (i = n; i > 0; i--)
		count = (count << 8) + run[i];

	return count;
}

ULONG CNtfsMtfReader::RunLength(PUCHAR run)
{
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
}

ULONGLONG CNtfsMtfReader::AttributeLength(PATTRIBUTE attr)
{
	return attr->Nonresident == FALSE ? PRESIDENT_ATTRIBUTE(attr)->ValueLength : ULONG(PNONRESIDENT_ATTRIBUTE(attr)->DataSize);
}

BOOL CNtfsMtfReader::bitset(PUCHAR bitmap, ULONGLONG i)
{
	return (bitmap[i >> 3] & (1 << (i & 7))) != 0;
}

BOOL CNtfsMtfReader::ReadFileRecord(PFILE_RECORD_HEADER Mft, ULONGLONG index, PFILE_RECORD_HEADER file)
{
	ULONG clusters = m_ClustersPerFileRecord;

	if (clusters > 0x80)
		clusters = 1;

	PUCHAR p = new UCHAR[m_BytesPerSector* m_SectorsPerCluster * clusters];
	ULONGLONG vcn = ULONGLONG(index) * m_BytesPerFileRecord/m_BytesPerSector/m_SectorsPerCluster;
	if(!ReadVCN(Mft, AttributeData, vcn, clusters, p))
		return FALSE;
	LONG m = (m_SectorsPerCluster * m_BytesPerSector/m_BytesPerFileRecord) - 1;
	ULONG n = m > 0 ? (index & m) : 0;
	memcpy(file, p + n * m_BytesPerFileRecord, m_BytesPerFileRecord);
	delete [] p;
	FixupUpdateSequenceArray(file);
	return TRUE;
}

BOOL CNtfsMtfReader::ReadVCN(PFILE_RECORD_HEADER file, ATTRIBUTE_TYPE type,ULONGLONG vcn, ULONG count, PVOID buffer)
{
	PATTRIBUTE attrlist = NULL;
	PNONRESIDENT_ATTRIBUTE attr = PNONRESIDENT_ATTRIBUTE(FindAttribute(file, type, 0));

	if (attr == 0 || (vcn < attr->LowVcn || vcn > attr->HighVcn))
	{
		return FALSE;
		//attrlist = FindAttribute(file, AttributeAttributeList, 0);
		//printf("**********************************************************");
		//DebugBreak();
		//TODO:基本遇不到这样的情况
	}
	return ReadExternalAttribute(attr, vcn, count, buffer, TRUE);
}

ULONGLONG CNtfsMtfReader::GetFileCount()
{
	NTFS_VOLUME_DATA_BUFFER ntfsVolData;
	DWORD dwWritten = 0;
	BOOL bDioControl = DeviceIoControl(m_Handle, FSCTL_GET_NTFS_VOLUME_DATA, NULL, 0, &ntfsVolData, sizeof(ntfsVolData), &dwWritten, NULL);
	return ntfsVolData.MftValidDataLength.QuadPart / m_BytesPerFileRecord;
}


const PFILENAME_ATTRIBUTE CNtfsMtfReader::ReadMtfFileNameAttribute(ULONGLONG ReferenceNumber)
{
	return ReadFileNameInfoEx(ReferenceNumber);
}
/*
BOOL CNtfsMtfReader::ReadMtfFileData(ULONGLONG ReferenceNumber, PUCHAR Buffer, ULONG bufferSize)
{
	if(m_hVolume == INVALID_HANDLE_VALUE)
		return NULL;

	if(!m_Mft){
		InitMftReader();
	}

	ZeroMemory(m_File, m_BytesPerFileRecord);

	if(!ReadFileRecord(m_Mft, ReferenceNumber, m_File)){
		return FALSE;
	}

	if (m_File->Ntfs.Type == 'ELIF'){
		if((m_File->Flags & 1) == 0)
			return NULL;

		PATTRIBUTE attr = FindAttribute(m_File, AttributeData, 0);
		if(attr){
			return ReadAttributeData(attr, Buffer, bufferSize);
		}
	}

	return FALSE;
}
*/
PFILENAME_ATTRIBUTE CNtfsMtfReader::ReadFileNameInfoEx(ULONGLONG ReferenceNumber)
{
	ZeroMemory(m_File, m_BytesPerFileRecord);

	if(!ReadFileRecord(m_Mft, ReferenceNumber, m_File)){
		return NULL;
	}

	if (m_File->Ntfs.Type == 'ELIF'){
		if((m_File->Flags & 1) == 0)
			return NULL;

		PVOID nameAttr = FindAttribute(m_File, AttributeFileName, 0);
		if(!nameAttr)
			return NULL;

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
		return NULL;
}
/*
BOOL CNtfsMtfReader::ReadAttributeData(PATTRIBUTE attr, PUCHAR Buffer, ULONG bufferSize)
{
	PRESIDENT_ATTRIBUTE rattr = NULL;
	PNONRESIDENT_ATTRIBUTE nattr = NULL;
	
	if (attr->Nonresident == FALSE){
		rattr = PRESIDENT_ATTRIBUTE(attr);
		if(bufferSize == 0)
			bufferSize = rattr->ValueLength;
		else if(bufferSize > rattr->ValueLength)
			bufferSize = rattr->ValueLength;
		memcpy(Buffer, Padd(rattr, rattr->ValueOffset), bufferSize);
		return TRUE;
	}else{
		BOOL result = TRUE;
		nattr = PNONRESIDENT_ATTRIBUTE(attr);
		ULONG nSector = bufferSize / m_BytesPerSector;
	
		PRUN_LINKE pRun = BuildRun(nattr);
		PRUN_LINKE pTmp = pRun;
		while(pRun){
			if(pRun->lcn > 0 && nSector > 0){
				ULONG n = pRun->size * m_BytesPerSector * m_SectorsPerCluster;
				ULONG readcount = n > nSector ? nSector : n;
				if(ReadSector(pRun->lcn * m_SectorsPerCluster, readcount, Buffer))
					nSector -= readcount;
				else
					result = FALSE;

				Buffer += readcount * m_BytesPerSector;
			}

			pTmp = pRun->next;
			delete pRun;
			pRun = pTmp;
		}
		return result;
	}
	
	return TRUE;
}
*/
/*
BOOL CNtfsMtfReader::OpenFile(ULONGLONG ReferenceNumber, PFILE_HANDLE* hFile)
{
	if(m_hVolume == INVALID_HANDLE_VALUE || !hFile)
		return FALSE;

	if(!m_Mft){
		InitMftReader();
	}
	PFILE_RECORD_HEADER pFile = (PFILE_RECORD_HEADER)new UCHAR[m_BytesPerFileRecord];
	ZeroMemory(pFile, m_BytesPerFileRecord);

	BOOL result = FALSE;
	do 
	{
		if(!ReadFileRecord(m_Mft, ReferenceNumber, (PFILE_RECORD_HEADER)pFile)){
			break;
		}

		if (pFile->Ntfs.Type == 'ELIF'){
			if((pFile->Flags & 1) == 0 || (pFile->Flags & 2) == 2)
				break;

			PATTRIBUTE attr = FindAttribute(pFile, AttributeData, 0);
			if(!attr){
				break;
			}

			*hFile = new FILE_HANDLE;
			ZeroMemory(*hFile, sizeof(FILE_HANDLE));
			(*hFile)->FileReferenceNumber = ReferenceNumber;
			(*hFile)->pFile = (PFILE_RECORD_HEADER)pFile;
			(*hFile)->DataAttr = attr;
			(*hFile)->DataSize = AttributeLength(attr);
			if(attr->Nonresident)
				(*hFile)->Run = BuildRun((PNONRESIDENT_ATTRIBUTE)attr);
			else
				(*hFile)->Run = NULL;
		}
		result = TRUE;
	} while (FALSE);

	//delete [] pFile;

	return result;

}

BOOL CNtfsMtfReader::ReadFile(PFILE_HANDLE hFile, PVOID Buffer, ULONG BufferLength, ULONGLONG Position, PULONG ByteReaded)
{
	if(!hFile || !hFile->DataAttr)
		return FALSE;

	PATTRIBUTE attr = hFile->DataAttr;
	PRESIDENT_ATTRIBUTE rattr = NULL;
	PNONRESIDENT_ATTRIBUTE nattr = NULL;
	ULONG readLength;

	if (!attr->Nonresident){
		rattr = PRESIDENT_ATTRIBUTE(attr);
		if(Position >= rattr->ValueLength)
			return FALSE;
		else{
			readLength = rattr->ValueLength - Position > BufferLength ? BufferLength : rattr->ValueLength - Position;
			memcpy(Buffer, Padd(rattr, rattr->ValueOffset + Position), readLength);
			if(ByteReaded)
				*ByteReaded = readLength;
			return TRUE;
		}
	}else{
		nattr = PNONRESIDENT_ATTRIBUTE(attr);
		ULONG nbS = Position / m_Bpb.BytesPerSector, nS = BufferLength / m_Bpb.BytesPerSector, nrS = 0, ntS = 0;

		PRUN_LINKE pRun = hFile->Run;
		PRUN_LINKE pTmp = pRun;

		while(pRun){
			if(nbS < pRun->size * m_Bpb.SectorsPerCluster){
				nrS = pRun->size * m_Bpb.SectorsPerCluster - nbS;
				nrS = nrS > nS ? nS : nrS;
				if(ReadSector(pRun->lcn * m_Bpb.SectorsPerCluster + nbS, nrS, Buffer)){
					nS -= nrS;
					ntS += nrS;
				}else
					return FALSE;
				nbS += nrS;
			}else
				nbS -= pRun->size * m_Bpb.SectorsPerCluster;
			
			if(nS <= 0)
				break;
			pRun = pRun->next;

		}
		if(ByteReaded)
			*ByteReaded = ntS * m_Bpb.BytesPerSector;
		return TRUE;
	}
}

BOOL CNtfsMtfReader::CloseFile(PFILE_HANDLE hFile)
{
	if(!hFile)
		return FALSE;
	if(hFile->pFile)
		delete [] hFile->pFile;
	PRUN_LINKE run = hFile->Run;
	while(run){
		PRUN_LINKE tmp = run->next;
		delete run;
		run = tmp;
	}
	delete hFile;
	return TRUE;
}
*/
void CNtfsMtfReader::ZeroMember()
{
	__super::ZeroMember();

	m_Mft = NULL;
	m_MftBitmap = NULL;
	m_File = NULL;
	m_FileCount = 0;
	m_SectorsPerCluster = 0;
	m_BytesPerFileRecord = 0;
	m_ClustersPerFileRecord = 0;
	m_BytesPerClusters = 0;

	ZeroMemory(&m_cache_info, sizeof(m_cache_info));	
}

PFILE_INFO CNtfsMtfReader::FileAttribute2Info(PFILENAME_ATTRIBUTE name)
{
	if(!name)
		return NULL;

	static PFILE_INFO aFileInfo =(PFILE_INFO) new UCHAR[sizeof(FILE_INFO) + MAX_PATH * sizeof(WCHAR)];
	ZeroMemory(aFileInfo, sizeof(FILE_INFO) + MAX_PATH * sizeof(WCHAR));
	aFileInfo->AllocatedSize = name->AllocatedSize;
	aFileInfo->ChangeTime = name->ChangeTime;
	aFileInfo->CreationTime = name->CreationTime;
	aFileInfo->DataSize = name->DataSize;
	aFileInfo->DirectoryFileReferenceNumber = name->DirectoryFileReferenceNumber;
	aFileInfo->FileAttributes = name->FileAttributes;
	aFileInfo->LastAccessTime = name->LastAccessTime;
	aFileInfo->LastWriteTime = name->LastWriteTime;
	aFileInfo->NameLength = name->NameLength;
	memcpy(aFileInfo->Name, name->Name, name->NameLength * sizeof(WCHAR));
	//wcscpy_s(aFileInfo->Name,name->NameLength * sizeof(WCHAR), name->Name);
	return aFileInfo;
}

bool CNtfsMtfReader::Init()
{
	if(__super::Init()){
		PUCHAR pBpb = new UCHAR[m_BytesPerSector];
		if(!ReadSector(0, 1, pBpb))
		{
#ifdef _DEBUG
			printf("Read Boot sector error code = %d \n", GetLastError());
#endif
			return false;
		}
		m_BytesPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord < 0x80
			? ((PNTFS_BPB)pBpb)->ClustersPerFileRecord * ((PNTFS_BPB)pBpb)->SectorsPerCluster
			* ((PNTFS_BPB)pBpb)->BytesPerSector: 1 << (0x100 - ((PNTFS_BPB)pBpb)->ClustersPerFileRecord);

		m_SectorsPerCluster = ((PNTFS_BPB)pBpb)->SectorsPerCluster;
		m_ClustersPerFileRecord = ((PNTFS_BPB)pBpb)->ClustersPerFileRecord;

		m_BytesPerClusters = m_BytesPerSector * m_SectorsPerCluster;

		if(!m_Mft)
			m_Mft = PFILE_RECORD_HEADER(new UCHAR[m_BytesPerFileRecord]);
		ZeroMemory(m_Mft, m_BytesPerFileRecord);
		if(!ReadSector((((PNTFS_BPB)pBpb)->MftStartLcn)*(m_SectorsPerCluster), (m_BytesPerFileRecord)/(m_BytesPerSector), m_Mft)){
			return 0;
		}
		FixupUpdateSequenceArray(m_Mft);
		PATTRIBUTE attrBitmap = FindAttribute(m_Mft, AttributeBitmap, 0);
		ULONGLONG allocSize = AttributeLengthAllocated(attrBitmap);
		if(m_MftBitmap)
			delete m_MftBitmap;
		m_MftBitmap = new UCHAR[(ULONG)allocSize];
		if(!ReadAttribute(attrBitmap, m_MftBitmap)){//$MFT元文件中的$Bitmap属性，此属性中标识了$MFT元文件中MFT项的使用状况
			return 0;
		}
		if(!m_File)
			m_File = PFILE_RECORD_HEADER(new UCHAR[m_BytesPerFileRecord]);
		m_FileCount = AttributeLength(FindAttribute(m_Mft, AttributeData, 0))/m_BytesPerFileRecord; //MTF的总项数
		if (m_cache_info.g_pb){
			delete [] m_cache_info.g_pb;
			m_cache_info.g_pb = NULL;
			m_cache_info.cache_lcn_begin = 0;
			m_cache_info.cache_lcn_count = 0;
			m_cache_info.cache_lcn_orl_begin = 0;
			m_cache_info.cache_lcn_total = 0;
		}

		delete [] pBpb;

		return true;
	}else
		return false;
}

const PFILE_INFO CNtfsMtfReader::GetFileInfo(ULONGLONG ReferenceNumber)
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
	ULONG br;
	return DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, outStatus, sizeof(USN_JOURNAL_DATA), &br, NULL);
}

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
			ULONG BytesReturned = 0;
			READ_USN_JOURNAL_DATA rujd = {LastUsn == 0 ? qujd.FirstUsn : LastUsn , 
				USN_REASON_CLOSE/*USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_FILE_CREATE | USN_REASON_RENAME_OLD_NAME0xffffffff/*to get all reason*/, 
				0, 0, 0, qujd.UsnJournalID};
			memset(buffer, 0, USN_PAGE_SIZE);
			ULONG cb = 0;
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

BOOL CNtfsMtfReader::CreateUsnJournal(HANDLE hVolume)
{
	CREATE_USN_JOURNAL_DATA cujd = {1024 * 1024 * 100, 1024 * 1024 * 20};
	ULONG br;
	return DeviceIoControl(hVolume, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, &br, NULL);
}

BOOL CNtfsMtfReader::DeleteUsnJournal(HANDLE hVolume)
{
	USN_JOURNAL_DATA uujd = {0};
	if(!QueryUsnStatus(hVolume, &uujd))
		return FALSE;
	ULONG br;
	DELETE_USN_JOURNAL_DATA dujd = {uujd.UsnJournalID, USN_DELETE_FLAG_DELETE || USN_DELETE_FLAG_NOTIFY};
	return DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL, &dujd, sizeof(dujd), NULL, 0, &br, NULL);
}

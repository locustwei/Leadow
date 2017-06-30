#include "DiskVolume.h"
#include <Shlwapi.h>

/*
 *	线程类型
 */
enum THREAD_TYPE{  
	TT_CREATE_DUMP,          //创建转存文件线程
	TT_LISTEN_CHANGE,        //监听文件修改线程
	TT_SEARCH_FILE,          //搜索文件线程
	TT_FIND_DUPLICATE        //搜索重复文件
};

/*
 *	线程参数
 */
typedef struct THTREAD_CONTEXT{
	THREAD_TYPE type;
	PVOID pParam;
	PVOID pResult;
}*PTHTREAD_CONTEXT;

enum CALLBACK_OPTION{
	EK_FILTER_RECORD = 0,
	EK_FIND_DUPLICATE_FILE = 1,
	EK_BINSEARCH = 2
};

/*
 *	枚举文件记录函数参数
 */
typedef struct ENUM_RECORD_CONTEXT{
	CALLBACK_OPTION op;   
	RECORD_FIELD_CLASS* c;
	PVOID pResult;
	PVOID Param;
}*PENUM_RECORD_CONTEXT;

//************************************
// Method:    CompareFileRecord
// FullName:  文件记录比较,返回大于、等于、小于
// Access:    private 
// Returns:   LONG
// Qualifier:
// Parameter: PMFTFILERECORD pSource
// Parameter: RECORD_FIELD_CLASS * c  比较文件记录的域
// Parameter: PMFTFILERECORD pRecord
//************************************
LONG CDiskVolume::CompareFileRecord(PMFTFILERECORD pSource, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord)
{
	LONGLONG r = 0;

	while(*c){
		switch(*c){
		case RFC_DIR:
			r = pSource->IsDir - pRecord->IsDir;
			break;
		case RFC_DS:
			r = pSource->DataSize - pRecord->DataSize;
			break;
		case RFC_DRN:
			r = pSource->DirectoryFileReferenceNumber - pRecord->DirectoryFileReferenceNumber;
			break;
		case RFC_NAME:
			r = _wcsnicmp(pSource->Name, pRecord->Name, pSource->NameLength > pRecord->NameLength ? pSource->NameLength : pRecord->NameLength);
			break;
		case RFC_MD5:
			r = memcmp(pSource->MD5, pRecord->MD5, MD5_CODE_LEN);
			break;
		default:
			r = 1;
			break;
		}
		if(r)
			break;
		c++;
	}

	if(r > 0)
		r = 1;
	else if(r < 0)
		r = -1;
	return (LONG)r;
}

//************************************
// Method:    FilterRecord
// FullName:  文件记录过滤（文件是否不符合过滤条件）
// Access:    private 
// Returns:   BOOL
// Qualifier:
// Parameter: PFILE_FILTER pFilter
// Parameter: PMFTFILERECORD pRecord
//************************************
BOOL CDiskVolume::FilterRecord(PFILE_FILTER pFilter, PMFTFILERECORD pRecord)
{ //return TRUE需要被过滤；
	
	if(!pFilter || !pRecord)
		return FALSE;

	if(pFilter->onlyFile && pRecord->IsDir)
		return TRUE;
	if(pFilter->beginSize >= 0 && pFilter->beginSize > (LONGLONG)pRecord->DataSize)    //大于字节
		return TRUE;
	if(pFilter->endSize >= 0 && pFilter->endSize < (LONGLONG)pRecord->DataSize)        //小于字节
		return TRUE;

	PCWSTR* pName = pFilter->pExcludeNames;  //文件名不包含
	while(pName && *pName){                          
		if(PathMatchSpec(pRecord->Name, *pName)){
			return TRUE;
		}
		pName ++;
	}

	pName = pFilter->pIncludeNames;       //文件名包含
	BOOL match = pName == NULL;
	while(pName && *pName){
		if(PathMatchSpec(pRecord->Name, *pName)){
			match = TRUE;
			break;
		}
		pName ++;
	}
	if(!match)
		return TRUE;

	if(pFilter->ParentDircetoryReferences){            //处在目录

		vector<ULONGLONG> ParentDir;
		ULONGLONG DirectoryReference = pRecord->DirectoryFileReferenceNumber;
		MFTFILERECORD mfr = {0};
		while(DirectoryReference){ // 包含子目录
			ParentDir.push_back(DirectoryReference);

			if(m_MftFile->ReadRecord(DirectoryReference, (PUCHAR)&mfr, sizeof(mfr)))
				DirectoryReference = mfr.DirectoryFileReferenceNumber;
			else
				DirectoryReference = 0;
		}

		PFILTER_DIRECTORY* pReference = pFilter->ParentDircetoryReferences;		

		while(*pReference){      //不包含子目录
			if(!(*pReference)->sub){
				if(pRecord->DirectoryFileReferenceNumber == (*pReference)->DircetoryReference){
					return FALSE;
				}else{
					for(size_t i=0; i<ParentDir.size(); i++)
						if(ParentDir[i] == (*pReference)->DircetoryReference)
							return FALSE;
				}
			}
			pReference++;
		}

	}

	return FALSE;
}


//************************************
// Method:    DoDuplicateFile
// FullName:  比较文件是否相同，并加入到重复文件列表中
// Access:    private 
// Returns:   LONG
// Qualifier:
// Parameter: vector<vector<PMFTFILERECORD> * > * pFiles  //重复文件列表
// Parameter: PMFTFILERECORD pPrv
// Parameter: RECORD_FIELD_CLASS * c
// Parameter: PMFTFILERECORD pRecord
// Parameter: USHORT Length
//************************************
/*
LONG CDiskVolume::DoDuplicateFile(vector<vector<PMFTFILERECORD> *>* pFiles, PMFTFILERECORD pPrv, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord, USHORT Length)
{
	BOOL equ = CompareFileRecord(pPrv, c, pRecord) == 0;

	vector<PMFTFILERECORD>* items = NULL;
	
	if(pFiles->size() > 0)
		items = pFiles->at(pFiles->size() - 1);
	 
	if(equ){        //与上一记录相同
		if(!items){
			items = new vector<PMFTFILERECORD>();
			pFiles->push_back(items);
		}
		
		try
		{
			PMFTFILERECORD pTmp;
			if(items->size() == 0){
				Length = sizeof(MFTFILERECORD) + (pPrv->NameLength + 1) * sizeof(WCHAR) - sizeof(WCHAR) * (MAX_PATH + 1);
				pTmp = (PMFTFILERECORD)new PUCHAR[Length];
				memcpy(pTmp, pPrv, Length);
				items->push_back(pTmp);
			}
			pTmp = (PMFTFILERECORD)new PUCHAR[Length];
			memcpy(pTmp, pRecord, Length);
			items->push_back(pTmp);
		}catch (...){
			return FALSE;
		}

	}else{
		CopyMemory(pPrv, pRecord, Length);
		if(items && items->size() > 0){
			items = new vector<PMFTFILERECORD>();
			pFiles->push_back(items);
		}
	}


	return TRUE;
}
*/
CDiskVolume::CDiskVolume(void):
	m_ListenThread(),
	m_DumpThread(),
	m_SearchThread()
{
	m_hVolume = INVALID_HANDLE_VALUE;
	m_MftReader = NULL;
	m_MftFile = NULL;
	m_Holder = NULL;
	m_DumpThread.SetFreeOnTerminate(false);
	m_ListenThread.SetFreeOnTerminate(false);
	m_SearchThread.SetFreeOnTerminate(false);

	InitializeCriticalSection(&m_cs);

	ZeroMemory(&m_MftInfo, sizeof(m_MftInfo));
}


CDiskVolume::~CDiskVolume(void)
{
//	CleanDuplicateFiles();
	m_ListenThread.Terminate(100);
	m_DumpThread.Terminate(100);
	m_SearchThread.Terminate(100);

	if(m_MftReader){
		delete m_MftReader;
		m_MftReader = nullptr;
	}

	if(m_MftFile){
		m_MftFile->WriteOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
		delete m_MftFile;
		m_MftFile = nullptr;
	}

	for(ULONG i=0; i<m_Indexs.size(); i++){
		delete [] m_Indexs[i].Index;
	}
	m_Indexs.clear();

	DeleteCriticalSection(&m_cs);

	if(m_hVolume != INVALID_HANDLE_VALUE)
		CloseHandle(m_hVolume);
}

BOOL CDiskVolume::OpenVolume(PWSTR wsz_guid)
{
	if(m_hVolume != INVALID_HANDLE_VALUE || wsz_guid == NULL)
		return FALSE;
	
	WCHAR Names[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(Names); 
	GetVolumePathNamesForVolumeName(wsz_guid, Names, cb, &cb);
	if(wcslen(Names)>0)
		Names[wcslen(Names) - 1] = '\0';
	wsz_guid[wcslen(wsz_guid) - 1] = '\0';

	if(OpenVolumeHandle(wsz_guid)){
		m_VolumeGuid = wsz_guid;
		m_VolumePath = Names;
	}

	return m_hVolume != INVALID_HANDLE_VALUE;
}

//************************************
// Method:    CreateMftDump
// FullName:  创建/更新磁盘文件转存文件。
// Access:    virtual public 
// Returns:   BOOL
// Qualifier:
// Parameter: BOOL async
//************************************
BOOL CDiskVolume::UpdateMftDump(BOOL async)
{
	if(m_hVolume == INVALID_HANDLE_VALUE || !m_MftFile || !m_MftReader)
		return FALSE;

	if(!m_MftReader->IsValid() || !m_MftFile->IsValid()){
		return FALSE;
	}

	if (m_DumpThread.IsAlive())
		return false;

	BOOL result;

 	if(m_MftInfo.FileCount == 0){
		if(async){
			PTHTREAD_CONTEXT pContext = new THTREAD_CONTEXT();
			pContext->type = TT_CREATE_DUMP;
			CThread* thread = new CThread(this, (WPARAM)pContext);
			thread->Start();

			result = TRUE;
		}else
			result = CreateMftDump();
 	}else{
 		if(async){
 			ListenFileChange();
 			result = TRUE;
 		}else
 			result = UpdateFiles();
 	}

	return result;
}
/*
 *	加载转存文件
 *  存储路径
 */
BOOL CDiskVolume::SetDumpFilePath(PCWSTR wsz_path)
{
	if (m_hVolume == INVALID_HANDLE_VALUE)
		return FALSE;

	if (!wsz_path)
	{
		return m_MftFile->OpenFile(NULL);
	}
	if(!wsz_path || !PathFileExists(wsz_path))
		return FALSE;

	//转存文件名
	if(!m_VolumeGuid.empty()){
		PWSTR s = (PWSTR)m_VolumeGuid.c_str();
		s += 10;
		m_MftDumpFileName += s;
	}else{
		m_MftDumpFileName += m_VolumePath;
		m_MftDumpFileName.replace_all('\\', 'A');
		m_MftDumpFileName.replace_all(':', 'B');
	}
	m_MftDumpFileName.insert(0, wsz_path);
	m_MftDumpFileName += L".idx";

	//读取/创建转存文件
	if(!m_MftFile->OpenFile(m_MftDumpFileName.c_str())){
		if(PathFileExists(m_MftDumpFileName.c_str()))
			if(DeleteFile(m_MftDumpFileName.c_str())){
				if(m_MftFile->OpenFile(m_MftDumpFileName.c_str()))
					return TRUE;
			}
		return FALSE;
	}else
		m_MftFile->ReadOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
	
	return TRUE;
}
/*
 *	创建Mft转存文件
 */
BOOL CDiskVolume::CreateMftDump()
{
	ULONGLONG count = 0;
	if(m_hVolume == INVALID_HANDLE_VALUE || !m_MftReader || !m_MftReader->IsValid() || !m_MftFile || !m_MftFile->IsValid())
		return FALSE;

	lock();
	try{
		m_MftFile->DeleteAllRecord();

		count = m_MftReader->EnumFiles(NULL);
		m_MftInfo.FileCount = count;
		m_MftInfo.LastUsn = m_MftReader->GetLastUsn();

		m_MftFile->WriteOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
	}catch(...){
		unlock();
		return FALSE;
	}
	unlock();
	return count > 0;
	
}
/*
 *	根据Usn Journal记录更新转存文件
 */
BOOL CDiskVolume::UpdateFiles()
{
	lock();
	try
	{
		USN usn = m_MftReader->UpdateFiles(m_MftInfo.LastUsn, NULL);
		if(usn){
			m_MftInfo.LastUsn = usn;
			m_MftFile->WriteOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
		}
	}catch(...){
		unlock();
		return FALSE;
	}
	unlock();

	return TRUE;
}

BOOL CDiskVolume::OpenVolumePath(const PWSTR wsz_volume)
{
	if(m_hVolume != INVALID_HANDLE_VALUE || wsz_volume == NULL)
		return FALSE;

	stringxw path = wsz_volume;
	if(wsz_volume[wcslen(wsz_volume)-1] != '\\')
		path += '\\';
	WCHAR guid[MAX_PATH + 1] = {0};
	ULONG cb = sizeof(guid); 
	GetVolumeNameForVolumeMountPoint(path.c_str(), guid, cb);
	if(wcslen(guid)>0)
		guid[wcslen(guid) - 1] = '\0';

	path.erase(path.length() - 1, 1);
	
	m_VolumePath = path;

	path.insert(0, L"\\\\.\\");
	if(OpenVolumeHandle(path.c_str())){
		m_VolumeGuid = guid;
	}

	return TRUE;
}

/*
 *	根据文件索引获取完整文件名（包含路径）
 */
ULONG CDiskVolume::GetFullFileName(ULONGLONG FileReferenceNumber, PWSTR wsz_Name, ULONG nameLength)
{
	if(m_MftFile == NULL || FileReferenceNumber == 0)
		return 0;
	
	ULONG length = 0;
	stringxw s;
	ULONGLONG ReferenceNumber = FileReferenceNumber;
	while(ReferenceNumber != 0){
		MFTFILERECORD record = {0};
		if(m_MftFile->ReadRecord(ReferenceNumber, (PUCHAR)&record, sizeof(record)) == 0)
			break;
		if((record.DirectoryFileReferenceNumber) == 0 || (record.DirectoryFileReferenceNumber) == ReferenceNumber)
			break;
		if(wsz_Name && (length + record.NameLength) < nameLength){
			s.insert(0, record.Name, record.NameLength);
			s.insert(0, L"\\");
			length ++;
//			CopyMemory(wsz_Directory + length, record.Name, record.NameLength * sizeof(WCHAR));
		}else
			break;
		length += record.NameLength;
		ReferenceNumber = record.DirectoryFileReferenceNumber;
	}
	s.insert(0, m_VolumePath.c_str(), m_VolumePath.length());
	CopyMemory(wsz_Name, s.c_str(), s.length() * sizeof(WCHAR));
	return length;
}
/*
 *	启动线程监视文件变更，及时更新转存文件
 */
BOOL CDiskVolume::ListenFileChange()
{
	if(m_hVolume == INVALID_HANDLE_VALUE || !m_MftFile)
		return FALSE;

	PTHTREAD_CONTEXT pContext = new THTREAD_CONTEXT();
	pContext->type = TT_LISTEN_CHANGE;
	CThread* thread = new CThread(this, TT_LISTEN_CHANGE);
	thread->Start();
	return TRUE;
}

BOOL CDiskVolume::OpenVolumeHandle(PCWSTR wsz_path)
{
	m_hVolume = CreateFile(wsz_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if(m_hVolume != INVALID_HANDLE_VALUE){
		if(m_MftReader){
			delete m_MftReader;
			m_MftReader = NULL;
		}

		m_MftReader = CMftReader::CreateReader(m_hVolume);
		m_MftReader->SetHolder(this);
		m_MftFile = new CRecordFile(this);
	}else
	{
#ifdef _DEBUG
		printf("Open disk error code = %d\n", GetLastError());
#endif
	}

	//PVOLUME_DISK_EXTENTS p = (PVOLUME_DISK_EXTENTS)new UCHAR[1000];
	//ZeroMemory(p, 1000);

	//DeviceIoControl(m_hVolume, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, NULL, 0, p, 1000, NULL, NULL);

	//printf("%x, %d %lld, %lld", IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, p->NumberOfDiskExtents, p->Extents[0].StartingOffset, p->Extents[0].ExtentLength);

	return m_hVolume != INVALID_HANDLE_VALUE;
}

PCWSTR CDiskVolume::GetVolumeGuid()
{
	return m_VolumeGuid.c_str();
}

PCWSTR CDiskVolume::GetVolumePath()
{
	return m_VolumePath.c_str();
}

PCWSTR CDiskVolume::GetDumpFileName()
{
	return m_MftDumpFileName.c_str();
}

/*
 *	创建文件索引（排序）
 */
PINDEX_RECORD CDiskVolume::CreateIndex(RECORD_FIELD_CLASS* c, PFILE_FILTER pFilter)
{
	if(!m_MftFile )
		return NULL;

	vector<ULONGLONG> FilterResult;

	if(pFilter && (pFilter->pExcludeNames || pFilter->pIncludeNames || pFilter->ParentDircetoryReferences || pFilter->beginSize >=0 || pFilter->endSize >=0 || pFilter->onlyFile)){
		//有过滤条件，把记录过滤出来
		ENUM_RECORD_CONTEXT context = {EK_FILTER_RECORD, c, &FilterResult, pFilter};

		m_MftFile->EnumRecord(&context);
	}else{
		//没有过滤条件则先看看索引是否已经存在
		RECORD_FIELD_CLASS* c1 = c;

		for (UCHAR i=0; i < m_Indexs.size(); i++){  //find exists index
			RECORD_FIELD_CLASS  *c2 = m_Indexs[i].c;
			while(*c1 && *c2){
				if(*c1 != *c2)
					break;
				c1++;
				c2++;
			}

			if(*c1 == 0 && *c2 == 0){
				return m_Indexs[i].Index;
			}
		}
	}


	PINDEX_RECORD idx = nullptr;
	lock();
	try
	{
		if(FilterResult.size())
			idx = m_MftFile->SortRecord(c, &FilterResult);
		else
			idx = m_MftFile->SortRecord(c);

		if(!idx)
			return NULL;

		if(!FilterResult.size()){  //如果是所有记录排序，保存下来以便下次使用
			INDEX_RECORD idxr = {0};
			idxr.Index = idx;
			RECORD_FIELD_CLASS* c1 = c;
			ULONG k = 0;
			while(*c1){
				k ++;
				c1++;
			}
			idxr.c = new RECORD_FIELD_CLASS[k + 1];
			for(ULONG i=0; i < k; i++){
				idxr.c[i] = c[i];
			}
			idxr.c[k] = RFC_NONE;
			m_Indexs.push_back(idxr);
		}
	}
	catch (...)
	{
		unlock();
		return 0;
	}
	unlock();

	return idx;
}
/*
 *	完整文件名转换成文件序号
 */
BOOL CDiskVolume::FullName2ReferenceNumber(PWSTR wsFullName, PULONGLONG ReferenceNumber)
{
	if(!wsFullName || wcslen(wsFullName) <= wcslen(GetVolumePath()))
		return FALSE;

	PWCHAR pTmp = wsFullName + wcslen(GetVolumePath()) + 1;
	ULONGLONG ParentReference = 0;
	
	RECORD_FIELD_CLASS c[] = {RFC_DRN, RFC_NAME, RFC_NONE};
	PINDEX_RECORD idx = CreateIndex(c, NULL);
	if(!idx)
		return 0;

	MFTFILERECORD record = {0};
	ENUM_RECORD_CONTEXT context = {EK_BINSEARCH, c, &record};

	ULONG i = 0;
	LONG cmpRt = 0;
	while(pTmp && pTmp[0] != '\0'){
		if(pTmp[0] == '\\'){
			record.Name[i++] = '\0';
			record.DirectoryFileReferenceNumber = ParentReference;
			ParentReference = m_MftFile->SearchRecord(idx, &context, &cmpRt);
			if(cmpRt)  //没找到
				return FALSE;
			ZeroMemory(record.Name, ARRAYSIZE(record.Name));
			i = 0;
		}else
			record.Name[i++] = pTmp[0];
		pTmp ++;
	}
	if(ReferenceNumber)
		*ReferenceNumber = ParentReference;
	return TRUE;
}
/*
 *	对文件MD5
 */

BOOL CDiskVolume::MakeFileMd5(ULONGLONG ReferenceNumber, BYTE* Md5Code)
{
#define DATA_BUFFER_LEN  1024 * 100
	BOOL result = FALSE;
	/*
	PUCHAR b = new UCHAR[DATA_BUFFER_LEN];
	ULONG readLength = DATA_BUFFER_LEN, cb;

	PFILE_HANDLE hFile = NULL;
	PMD5_HANDLE hMd5 = NULL;
	do 
	{
		if(!m_MftReader->OpenFile(ReferenceNumber, &hFile))
			break;
		if(Md5_Init(&hMd5))
			break;
		ULONGLONG FileLength = hFile->DataSize;
		ULONGLONG tlen = 0;
		while(TRUE){
			if(!m_MftReader->ReadFile(hFile, b, readLength, tlen, &cb) || cb == 0)
				break;
			if(cb > FileLength - tlen)
				cb = (ULONG)(FileLength - tlen);
			tlen += cb;

			Md5_HashData(hMd5, b, cb, NULL);

			if(tlen >= FileLength){
				break;
			}
		}

		result = TRUE;
	} while (FALSE);

	delete [] b;

	if(hMd5)
		Md5_Finsh(hMd5, Md5Code);
	if(hFile)
		m_MftReader->CloseFile(hFile);
	*/
	return result;

}

/*
 *	创建Mft转存文件回掉函数（逐个把Mft文件记录写到转存文件中）
 */
BOOL CDiskVolume::EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileName, PVOID Param)
{
	if(pFileName == NULL)
		return FALSE;
	MFTFILERECORD mfr = {0};
	//mfr.FileReferenceNumber = ReferenceNumber;
	mfr.IsDir = (pFileName->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
	mfr.DirectoryFileReferenceNumber = pFileName->DirectoryFileReferenceNumber & 0x0000FFFFFFFFFFFF;
	mfr.DataSize = pFileName->DataSize;
	mfr.NameLength = pFileName->NameLength;
	CopyMemory(mfr.Name, pFileName->Name, pFileName->NameLength * sizeof(WCHAR));

#ifdef _DEBUG
	//wprintf(L"%lld   %s \n", ReferenceNumber, mfr.Name);
#endif // _DEBUG
	
	m_MftFile->WriteRecord(ReferenceNumber, (PUCHAR)&mfr, sizeof(mfr) - sizeof(mfr.Name) + (pFileName->NameLength + 1) * sizeof(WCHAR));
	return !m_DumpThread.GetTerminated();

}
/*
 *	列举USN Journal记录回掉函数（更新Mft转存文件记录）
 */
BOOL CDiskVolume::EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param)
{
	ULONGLONG ReferenceNumber = record->FileReferenceNumber & 0x0000FFFFFFFFFFFF;

	MFTFILERECORD mfr = {0};

	if(record->Reason & USN_REASON_FILE_DELETE)      //删除文件
		m_MftFile->DeleteRecord(ReferenceNumber);
	else if(record->Reason & USN_REASON_FILE_CREATE){ // || record->Reason & USN_REASON_RENAME_NEW_NAME){ //新文件
		PFILE_INFO pFileName = m_MftReader->GetFileInfo(ReferenceNumber);
		if(pFileName){
			//mfr.FileReferenceNumber = ReferenceNumber;
			mfr.IsDir = (pFileName->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			mfr.DirectoryFileReferenceNumber = pFileName->DirectoryFileReferenceNumber & 0x0000FFFFFFFFFFFF;
			mfr.DataSize = pFileName->DataSize;
			mfr.NameLength = pFileName->NameLength;
			CopyMemory(mfr.Name, pFileName->Name, pFileName->NameLength * sizeof(WCHAR));
			if(mfr.DataSize && !mfr.IsDir)
				MakeFileMd5(ReferenceNumber, mfr.MD5);
			m_MftFile->WriteRecord(ReferenceNumber, (PUCHAR)&mfr, sizeof(mfr) - sizeof(mfr.Name) + (mfr.NameLength + 1) * sizeof(WCHAR));

#ifdef _DEBUG
			wprintf(L"%4d   %s -----\n", ReferenceNumber, mfr.Name);
#endif // _DEBUG
			
		}
		/*
		else{
			//mfr.FileReferenceNumber = ReferenceNumber;
			mfr.IsDir = (record->FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			mfr.DirectoryReferenceNumber = record->ParentFileReferenceNumber & 0x0000FFFFFFFFFFFF;
			mfr.NameLength = (UCHAR)(record->FileNameLength / sizeof(WCHAR));
			CopyMemory(mfr.Name, record->FileName, record->FileNameLength);

		}
		*/
	}else if(record->Reason & USN_REASON_RENAME_NEW_NAME){  //更名

		m_MftFile->ReadRecord(ReferenceNumber, (PUCHAR)&mfr, sizeof(mfr));
		//wprintf(L"%4d   %s -----\n", ReferenceNumber, mfr.Name);
		mfr.DirectoryFileReferenceNumber = record->ParentFileReferenceNumber  & 0x0000FFFFFFFFFFFF;
		mfr.NameLength = (UCHAR)(record->FileNameLength / sizeof(WCHAR));
		CopyMemory(mfr.Name, record->FileName, record->FileNameLength);
		mfr.Name[record->FileNameLength / sizeof(WCHAR)] = '\0';
		//wprintf(L"%4d   %s \n", ReferenceNumber, mfr.Name);
		m_MftFile->WriteRecord(ReferenceNumber, (PUCHAR)&mfr, sizeof(mfr) - sizeof(mfr.Name) + record->FileNameLength + sizeof(WCHAR));

	}else if(record->Reason & (USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_DATA_EXTEND)){  //文件内容修改
		USHORT cb = m_MftFile->ReadRecord(ReferenceNumber, (PUCHAR)&mfr, sizeof(mfr));
		if(mfr.IsDir || mfr.DataSize == 0)
			ZeroMemory(mfr.MD5, MD5_CODE_LEN);
		else
			MakeFileMd5(ReferenceNumber, mfr.MD5);       //DataSize error
		m_MftFile->WriteRecord(ReferenceNumber, (PUCHAR)&mfr, cb);
	}

	return !m_ListenThread.GetTerminated();	
}
/**
搜索文件回掉函数
**/
LONG CDiskVolume::EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param)
{
	PENUM_RECORD_CONTEXT context = (PENUM_RECORD_CONTEXT)Param;
	switch(context->op){
	case EK_FILTER_RECORD:  
		if(!FilterRecord((PFILE_FILTER)context->Param, (PMFTFILERECORD)pRecord))
		{

#ifdef _DEBUG
			printf("%lld %S\n", ReferenceNumber, ((PMFTFILERECORD)pRecord)->Name);
#endif // _DEBUG
		}
		return TRUE;
	case EK_BINSEARCH:
		return CompareFileRecord((PMFTFILERECORD)context->Param, context->c, (PMFTFILERECORD)pRecord);
	}
	return !m_SearchThread.GetTerminated();
}

/*
 *	创建索引比较函数
 */
LONG CDiskVolume::RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR p1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR p2, USHORT Length2, PVOID Param)
{
	RECORD_FIELD_CLASS* c = (RECORD_FIELD_CLASS*)Param;
	PMFTFILERECORD pRecord1 = (PMFTFILERECORD)p1;
	PMFTFILERECORD pRecord2 = (PMFTFILERECORD)p2;
	
	LONGLONG r = 0;

	while(*c){
		switch((*c)){
		case RFC_FN:
			//r = pRecord1->FileReferenceNumber - pRecord2->FileReferenceNumber;
			break;
		case RFC_DIR:
			r = pRecord2->IsDir - pRecord1->IsDir;
			break;
		case RFC_DS:
			r = pRecord1->DataSize - pRecord2->DataSize;
			break;
		case RFC_DRN:
			r = pRecord1->DirectoryFileReferenceNumber - pRecord2->DirectoryFileReferenceNumber;
			break;
		case RFC_NAME:
			r = _wcsnicmp(pRecord1->Name, pRecord2->Name, pRecord1->NameLength > pRecord2->NameLength ? pRecord1->NameLength : pRecord2->NameLength);
			break;
		case RFC_MD5:
			if(!pRecord1->IsDir && *(PULONGLONG)pRecord1->MD5 == 0){
				MakeFileMd5(ReferenceNumber1, pRecord1->MD5);
				m_MftFile->WriteRecord(ReferenceNumber1, p1, Length1);
			}
			if(!pRecord2->IsDir && *(PULONGLONG)pRecord2->MD5 == 0){
				MakeFileMd5(ReferenceNumber2, pRecord2->MD5);
				m_MftFile->WriteRecord(ReferenceNumber2, p2, Length2);
			}
			r = memcmp(pRecord1->MD5, pRecord2->MD5, MD5_CODE_LEN);
			break;
		}

		if(r != 0){
			break;
		}else{
			c++;
		}

	}
	if(r == 0)  //加快排序速度
		r = ReferenceNumber1 - ReferenceNumber2;

	if(r > 0)
		r = 1;
	else
		r = -1;
	return (LONG)r;
}

// void CDiskVolume::CleanDuplicateFiles()
// {
// 	for(ULONG i=0; i<DuplicateResult.size(); i++){
// 		vector<PMFTFILERECORD>* item = DuplicateResult.at(i);
// 		for (ULONG j=0; j<item->size(); j++){
// 			PMFTFILERECORD p = item->at(j);
// 			if(p)
// 				delete [] p;
// 		}
// 		delete item;
// 	}
// 	DuplicateResult.clear();
// }

VOID CDiskVolume::ThreadRun(CThread* Sender, WPARAM Param)
{
	PTHTREAD_CONTEXT pContext = (PTHTREAD_CONTEXT)Param;

	switch(pContext->type){
	case TT_LISTEN_CHANGE:
		RunListenFileChange();
		break;
	case TT_CREATE_DUMP:
		CreateMftDump();
		break;
	case TT_SEARCH_FILE:
		SearchFile((PFILE_FILTER)pContext->pParam);
		break;
	}
}

VOID CDiskVolume::OnThreadInit(CThread* Sender, WPARAM Param)
{
	
}

VOID CDiskVolume::OnThreadTerminated(CThread* Sender, WPARAM Param)
{
	PTHTREAD_CONTEXT pContext=(PTHTREAD_CONTEXT)Param;
	if(m_Holder){
		switch(pContext->type){
		case TT_LISTEN_CHANGE:
			break;
		case TT_CREATE_DUMP:
			m_Holder->OnRenewDump(this);
			break;
		case TT_SEARCH_FILE:
			m_Holder->OnSearchCompeleted(this);
			break;
		case TT_FIND_DUPLICATE:
			m_Holder->OnFindDupCompeleted(this);
			break;
		}
	}

	delete pContext;
}

BOOL CDiskVolume::SearchFile(PFILE_FILTER pFilter)
{
	if(!pFilter || !m_MftFile || !m_MftFile->IsValid())
		return FALSE;

	ULONGLONG count = 0;
	lock();
	try{
		ENUM_RECORD_CONTEXT context = {EK_FILTER_RECORD, NULL, nullptr, pFilter};

		m_MftFile->EnumRecord(&context);
	}catch(...){
		unlock();
		return FALSE;
	}

	if(m_Holder)
		m_Holder->OnSearchCompeleted(this);

	unlock();

#ifdef _DEBUG
//	printf("total %lld files  found %lld files \n", m_MftInfo.FileCount, Result->size());
#endif


	return TRUE;
}

BOOL CDiskVolume::SearchFile(PFILE_FILTER pFilter, BOOL asyn)
{
	if (!m_SearchThread.IsAlive())
		return false;

	if(asyn){
		PTHTREAD_CONTEXT pContext = new THTREAD_CONTEXT();	
		pContext->type = TT_SEARCH_FILE;
		pContext->pParam = pFilter;

		CThread* thread = new CThread(this, (WPARAM)pContext);

		thread->Start();

		return TRUE;
	}else{
		return SearchFile(pFilter);
	}

	return FALSE;
}

/*
 *	监视文件修改线程
 */
VOID CDiskVolume::RunListenFileChange()
{
	
	HANDLE hMonitor = INVALID_HANDLE_VALUE;
	if(!m_VolumePath.empty()){
		stringxw path = m_VolumePath;
		path += '\\';
		hMonitor = FindFirstChangeNotification(path.c_str(), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);
	}

	while(!m_ListenThread.GetTerminated()){
		if(hMonitor != INVALID_HANDLE_VALUE){
			WaitForSingleObject(hMonitor, INFINITE);
			  if(FALSE==FindNextChangeNotification(hMonitor))
				  break;
		}else
			m_ListenThread.Sleep(10*1000);


		//m_ListenThread.Sleep(10000);   //
		UpdateFiles();
	}

	if(hMonitor != INVALID_HANDLE_VALUE)
		FindCloseChangeNotification(hMonitor);

}

VOID CDiskVolume::SetHolper(IVolumeEvent* pHoler)
{
	m_Holder = pHoler;
}

BOOL  CDiskVolume::UnLoadDumpFile()
{
	return m_MftFile->SaveFile();	
}

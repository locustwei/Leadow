#include "RecordFile.h"
#include <Shlwapi.h>
#include <WtsApi32.h>

#define RECORD_BLOCK_SIZE BLOCK_REFERENCE_COUNT * sizeof(RECORD_POINTER)

#define FILE_PAGE_SIZE  1024 * 0xFFF

//#define INDEX_BLOCK_SIZE BLOCK_REFERENCE_COUNT * sizeof(INDEX_RECORD)

CRecordFile::CRecordFile(PRecordFileHolder Holder):
	m_RecordIndexBlocks()
{
	m_Holder = Holder;
	m_hFile = INVALID_HANDLE_VALUE;
	m_hFileMap = NULL;
	m_pMapStart = NULL;	
	m_Header = NULL;
}


CRecordFile::~CRecordFile(void)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		FreeSpace();
	}
	else
	{
		SaveFile();
		CloseHandle(m_hFile);
	}
}

//************************************
// Method:    OpenFile
// FullName:  创建/或打开一个文件/或不生成文件使用内存。
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR Filename
//************************************
BOOL CRecordFile::OpenFile(LPCTSTR Filename)
{
	if (Filename == NULL)
	{
		m_Header = new FILE_HEADER();
		ZeroMemory(m_Header, sizeof(FILE_HEADER));
		return TRUE;
	}
	else
	{
		BOOL Result = FALSE;
		do
		{
			if (m_hFile != INVALID_HANDLE_VALUE) {
				break;
			}

			DWORD op;
			if (PathFileExists(Filename))
				op = OPEN_EXISTING;
			else
				op = CREATE_NEW;

			m_hFile = CreateFile(Filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, op, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN, NULL);
			if (m_hFile == INVALID_HANDLE_VALUE) {
				break;
			}

			if (op == OPEN_EXISTING)
				if (!InitFileMapView())
					break;
			if (!Init())
				break;

			Result = TRUE;
		} while (FALSE);


		return Result;
	}
}

//************************************
// Method:    WriteRecord
// FullName:  添加或重写文件记录
// Access:    private 
// Returns:   BOOL
// Qualifier:
// Parameter: UINT64 ReferenceNumber
// Parameter: PUCHAR pRecord
// Parameter: USHORT Length
//************************************
BOOL CRecordFile::WriteRecord(UINT64 ReferenceNumber, PUCHAR pRecord, USHORT Length)
{
	PRECORD_POINTER rs = GetRecordPointer(ReferenceNumber);
	if(rs == NULL)
	{
		rs = AddFileReference(ReferenceNumber);
		if(rs == NULL)
			return FALSE;
	}

	if(rs->Pointer == 0 || rs->Length < Length) {
		UINT64 Pointer = AllocSpace(Length);  //分配空间可能引起内存影像重建
		if(Pointer == 0)
			return FALSE;
		rs = GetRecordPointer(ReferenceNumber);
		rs->Pointer = Pointer;
	}

	return WriteRecord(rs, pRecord, Length);
}

DWORD CRecordFile::WriteRecord(PRECORD_POINTER rs, PUCHAR pRecord, USHORT Length)
{
	if(!rs|| rs->Pointer == 0)
		return 0;

	PVOID pData;

	rs->Length = Length;
	pData = File2MapPointer(rs->Pointer);
	//pData->Length = Length;
	MoveMemory(pData, pRecord, Length);
	return Length;
	//return MyWriteFile(pRecord, Length, rs->Pointer);
}

//************************************
// Method:    Init
// FullName:  打开文件进行初始化。
// Access:    private 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL CRecordFile::Init()
{
	if(m_Header)
		return LoadRecordIndex();
	else
		return TRUE;
}

//************************************
// Method:    GetRecordPointer
// FullName:  获取记录序号的记录存储位置
// Access:    private 
// Returns:   CRecordFile::PRECORD_POINTER
// Qualifier:
// Parameter: UINT64 ReferenceNumber
//************************************
CRecordFile::PRECORD_POINTER CRecordFile::GetRecordPointer(UINT64 ReferenceNumber)
{
	DWORD m = (DWORD)(ReferenceNumber / BLOCK_REFERENCE_COUNT), n = (DWORD)(ReferenceNumber % BLOCK_REFERENCE_COUNT);
	if(m_RecordIndexBlocks.size() <= m)
		return NULL;
	return (PRECORD_POINTER)(File2MapPointer(m_RecordIndexBlocks[m] + n * sizeof(RECORD_POINTER)));
}

//************************************
// Method:    AddFileReference
// FullName:  添加记录序号（文件中记录序号是顺序存放，不能跳号每）
// Access:    private 
// Returns:   CRecordFile::PRECORD_POINTER
// Qualifier:
// Parameter: UINT64 ReferenceNumber
//************************************
CRecordFile::PRECORD_POINTER CRecordFile::AddFileReference(UINT64 ReferenceNumber)
{
	if (!m_Header)
		return nullptr;

	DWORD m = (DWORD)(ReferenceNumber / BLOCK_REFERENCE_COUNT), n = (DWORD)(ReferenceNumber % BLOCK_REFERENCE_COUNT);

	for (size_t i=m_RecordIndexBlocks.size(); i<=m; i++){
		UINT64 Tmp = 0;
		Tmp = AllocSpace(RECORD_BLOCK_SIZE + sizeof(DWORD));
		if(Tmp == 0)
			return NULL;
		*(PULONG)(File2MapPointer(Tmp + RECORD_BLOCK_SIZE)) = m_Header->LastReferenceBlock;
		m_Header->LastReferenceBlock = (DWORD)Tmp;
		m_RecordIndexBlocks.push_back(Tmp);
	}

	return GetRecordPointer(ReferenceNumber);
}

//************************************
// Method:    SetRecordPointer
// FullName:  修改记录位置/长度
// Access:    private 
// Returns:   void
// Qualifier:
// Parameter: UINT64 ReferenceNumber
// Parameter: DWORD Pointer
// Parameter: USHORT Length
//************************************
void CRecordFile::SetRecordPointer(UINT64 ReferenceNumber, DWORD Pointer, USHORT Length)
{
	PRECORD_POINTER ps = GetRecordPointer(ReferenceNumber);
	if(!ps)
		return;
	ps->Length = Length;
	ps->Pointer = Pointer;
}

//************************************
// Method:    ReadRecord
// FullName:  读取记录数据（Copy数据）
// Access:    private 
// Returns:   USHORT
// Qualifier:
// Parameter: UINT64 ReferenceNumber
// Parameter: PVOID Buffer
// Parameter: USHORT Length
//************************************
USHORT CRecordFile::ReadRecord(UINT64 ReferenceNumber, PVOID Buffer, USHORT Length)
{
	PRECORD_POINTER rs = GetRecordPointer(ReferenceNumber);
	if(rs == NULL || rs->Pointer == 0)
		return 0;
	
	if(!Buffer)
		return rs->Length;

	return ReadRecord(rs, Buffer, Length);

}

USHORT CRecordFile::ReadRecord(PRECORD_POINTER rs, PVOID Buffer, USHORT Length)
{
	USHORT cb = rs->Length > Length? Length : rs->Length;

	PVOID p = File2MapPointer(rs->Pointer);
	if(!p)
		return 0;

	CopyMemory(Buffer, p, cb);

	return cb;
}

//************************************
// Method:    DeleteRecord
// FullName:  删除记录（数据不被真正删除，因为序号不能中断）
// Access:    public 
// Returns:   VOID
// Qualifier:
// Parameter: UINT64 ReferenceNumber
//************************************
VOID CRecordFile::DeleteRecord(UINT64 ReferenceNumber)
{
	SetRecordPointer(ReferenceNumber, 0, 0);
}

//************************************
// Method:    EnumRecord
// FullName:  遍历文件数据。
// Access:    public 
// Returns:   UINT64
// Qualifier:
// Parameter: PVOID Param  回掉函数参数
// Parameter: PINDEX_RECORD pIndex  使用索引（可以为NULL）
// Parameter: UINT64 begin  其实记录号
// Parameter: UINT64 end   结束记录号
//************************************
UINT64 CRecordFile::EnumRecord(PVOID Param, PINDEX_RECORD pIndex, UINT64 begin, UINT64 end)
{
	if(m_Holder == NULL)
		return 0;

	UINT64 result = 0;
	BOOL b = TRUE;

	if(pIndex)
		return EnumRecordByIndex(Param, (PINDEX_RECORD)pIndex, begin, end);

	PRECORD_POINTER rs;

	UCHAR pRecord[MAX_RECORD_LENGTH] = {0}; // = new UCHAR[rs->Length];

	for(UINT64 i=0; i<GetLastReference(); i++){
		rs = GetRecordPointer(i); 
		if(!rs || rs->Pointer == 0 || rs->Length == 0){
			continue;
		}
		
		if(ReadRecord(rs, pRecord, rs->Length))
			if(!m_Holder->EnumFileRecordCallback(i, pRecord, rs->Length, Param)){
				break;
			}
			//delete pRecord;
		result ++;
	}
	return result;
}

UINT64 CRecordFile::EnumRecordByIndex(PVOID Param, PINDEX_RECORD pIndex, UINT64 begin, UINT64 end)
{
	UINT64 Reference, result = 0;
	PRECORD_POINTER rs;

	UCHAR pRecord[MAX_RECORD_LENGTH] = {0}; // = new UCHAR[rs->Length];
	if(end == 0)
		end = pIndex->count;
	for(UINT64 i=begin; i<end; i++){
		Reference = pIndex->Index[i]; 
		rs = GetRecordPointer(Reference); 
		//printf("%lld, %lld, %lld \n", i*BLOCK_REFERENCE_COUNT + j, Reference, (*pVector)[i].IndexBlock[j].IndexReference);
		if(!rs || rs->Pointer == 0 || rs->Length == 0){
			continue;
		}
		if(ReadRecord(rs, pRecord, rs->Length))
			if(!m_Holder->EnumFileRecordCallback(Reference, pRecord, rs->Length, Param)){
				break;
			}
			result ++;
	}
	return result;
}

typedef struct COMPARE_CONTEXT{
	CRecordFile* file;
	PRecordFileHolder func;
	PVOID param;
}*PCOMPARE_CONTEXT;

//************************************
// Method:    sortCompare
// FullName:  排序记录比较函数
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void * context
// Parameter: const void * id1
// Parameter: const void * id2
//************************************
int _cdecl sortCompare(void * context, const void * id1, const void *id2)
{
	CRecordFile* file = (CRecordFile*)((COMPARE_CONTEXT*)context)->file;
	CRecordFile::PRECORD_POINTER rs1 = file->GetRecordPointer(*(PULONGLONG)id1);
	CRecordFile::PRECORD_POINTER rs2 = file->GetRecordPointer(*(PULONGLONG)id2);

	if(!rs1 || rs1->Pointer == 0){
		if(!rs2 || rs2->Pointer == 0){
			if(*(PULONGLONG)id1 > *(PULONGLONG)id2)
				return 1;
			else
				return -1;
		}else
			return 1;
	} else if(!rs2 || rs2->Pointer == 0)
		return - 1;

	PUCHAR record1 = new UCHAR[rs1->Length];
	file->ReadRecord(rs1, record1, rs1->Length);
	PUCHAR record2 = new UCHAR[rs2->Length];
	file->ReadRecord(rs2, record2, rs2->Length);

	int result = ((COMPARE_CONTEXT*)context)->func->RecordComparer(*(PULONGLONG)id1, record1, rs1->Length, *(PULONGLONG)id2, record2, rs2->Length, ((COMPARE_CONTEXT*)context)->param);

	if(!result)
		result = (int)*(PULONGLONG)id1 - (int)*(PULONGLONG)id2;

	delete record1;
	delete record2;

	return result;
}

//************************************
// Method:    SortRecord
// FullName:  记录排序
// Access:    public 
// Returns:   PINDEX_RECORD
// Qualifier:
// Parameter: PVOID param
// Parameter: vector<UINT64> * pReferences
//************************************
PINDEX_RECORD CRecordFile::SortRecord(PVOID param, vector<UINT64>* pReferences)
{
	if(m_RecordIndexBlocks.empty() || !m_Holder)
		return NULL;
	PINDEX_RECORD result = NULL; 

	
	vector<UINT64> tmp;
	if(!pReferences){
		UINT64 count = GetLastReference(); //m_ReferenceBlocks.size() * BLOCK_REFERENCE_COUNT;
		tmp.reserve(count);
		for(UINT64 i = 0; i < count; i ++ ){
			PRECORD_POINTER rs = GetRecordPointer(i);
			if(rs && rs->Pointer)
				tmp.push_back(i);
		}
		pReferences = &tmp;
	}

	COMPARE_CONTEXT context = {this, m_Holder, param};
	qsort_s(pReferences->data(), pReferences->size(), sizeof(UINT64), &sortCompare, &context);

	//UINT64* ReferenceNumbers = ;
	result = new INDEX_RECORD;
	result->count = pReferences->size();
	result->Index = new UINT64[pReferences->size()];
	memcpy(result->Index, pReferences->data(), pReferences->size()*sizeof(UINT64));

	tmp.clear();

	return result;
}

//************************************
// Method:    WriteOptionRecord
// FullName:  添加或修改配置记录
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: UCHAR ReferenceNumber
// Parameter: PUCHAR pRecord
// Parameter: USHORT Length
//************************************
BOOL CRecordFile::WriteOptionRecord(UCHAR ReferenceNumber, PUCHAR pRecord, USHORT Length)
{
	if(!m_Header || ReferenceNumber >= MAX_OPTION)
		return FALSE;
	
	PRECORD_POINTER rs = &m_Header->OptionRecord[ReferenceNumber];

	if(rs->Pointer == 0 || rs->Length < Length) {
		UINT64 Pointer = AllocSpace(Length);  
		if(Pointer == 0)
			return FALSE;
		rs = &m_Header->OptionRecord[ReferenceNumber];
		rs->Pointer = Pointer;
	}

	return WriteRecord(rs, pRecord, Length);
}

//************************************
// Method:    ReadOptionRecord
// FullName:  读取配置记录
// Access:    public 
// Returns:   USHORT
// Qualifier:
// Parameter: UCHAR ReferenceNumber
// Parameter: PUCHAR Buffer
// Parameter: USHORT Length
//************************************
USHORT CRecordFile::ReadOptionRecord(UCHAR ReferenceNumber, PUCHAR Buffer, USHORT Length)
{
	if(!m_Header || ReferenceNumber >= MAX_OPTION)
		return 0;
	if(m_Header->OptionRecord[ReferenceNumber].Pointer == 0)
		return 0;
	return ReadRecord(&m_Header->OptionRecord[ReferenceNumber], Buffer, Length);
}
/*
BOOL CRecordFile::MyReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, DWORD Pointer)
{
	PVOID p = File2MapPointer(Pointer);
	if(!p)
		return FALSE;

	if(Pointer + nNumberOfBytesToRead <= m_Header->FileSize){
		CopyMemory(lpBuffer, p, nNumberOfBytesToRead);
		return TRUE;
	}else
		return FALSE;
}
*/
/*
DWORD CRecordFile::MyWriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, DWORD Pointer)
{
	PVOID p = File2MapPointer(Pointer);
	if(!p)
		return 0;

	CopyMemory(p, lpBuffer, nNumberOfBytesToWrite);
	return Pointer;
}
*/
//************************************
// Method:    LoadRecordIndex
// FullName:  读取文件中记录序号（初始化时）
// Access:    private 
// Returns:   BOOL
// Qualifier:
//************************************
BOOL CRecordFile::LoadRecordIndex()
{
	if(!m_Header || m_Header->LastReferenceBlock == 0)
		return FALSE;

	DWORD nRead = 0;
	DWORD Pointer = m_Header->LastReferenceBlock;
	
	while (Pointer) {
		m_RecordIndexBlocks.insert(m_RecordIndexBlocks.begin(), Pointer);
		Pointer = *(PULONG)(File2MapPointer(Pointer + RECORD_BLOCK_SIZE));
		//Tmp.PrvPointer = Pointer;
	}
	
	return TRUE;

}
/*
BOOL CMftFile::UpdateIndex(UCHAR Index, UINT64 ReferenceNumber, BinSearchCompare compare, PVOID param)
{
	if(Index<1 || Index>MAX_INDEX)
		return FALSE;

	vector<BLOCK_RECORD>* pVector;
	pVector = &m_IndexBlocks[Index - 1];

	if(pVector->empty())
		if(!BuilerIndex(Index, pVector, INDEX_BLOCK_SIZE))
			return FALSE;

	if(ReferenceNumber >= pVector->size() * BLOCK_REFERENCE_COUNT){
		UINT64 number = pVector->size() * BLOCK_REFERENCE_COUNT;
		while(pVector->size() * BLOCK_REFERENCE_COUNT <= ReferenceNumber){
			BLOCK_RECORD block = {0};
			block.Pointer = 0;
			block.IndexBlock = new INDEX_RECORD[BLOCK_REFERENCE_COUNT];
			for(int i = 0; i < BLOCK_REFERENCE_COUNT; i++){
				block.IndexBlock[i].IndexReference = number;
				block.IndexBlock[i].RecordReference = number;
				number ++;
			}
			block.PrvPointer = pVector->at(pVector->size() - 1).Pointer;
			block.Pointer = WriteBlockToFile(&block, INDEX_BLOCK_SIZE);
			m_Header.IndexPointer[Index] = block.Pointer;
			pVector->push_back(block);
		}

		UpdateHeader();
	}

	LONG cr;
	UINT64 indexReference = BinSearch(pVector, compare, param, &cr);
	UINT64 oldIndex = (*pVector)[ReferenceNumber / BLOCK_REFERENCE_COUNT].IndexBlock[ReferenceNumber % BLOCK_REFERENCE_COUNT].IndexReference;
	if(indexReference == oldIndex)
		return TRUE;

	int n = indexReference > oldIndex ? -1 : 1;
	UINT64 reference = (*pVector)[indexReference / BLOCK_REFERENCE_COUNT].IndexBlock[indexReference % BLOCK_REFERENCE_COUNT].RecordReference;
	for(UINT64 i=indexReference+n; ; i+=n){
		UINT64 tmp = (*pVector)[i / BLOCK_REFERENCE_COUNT].IndexBlock[i % BLOCK_REFERENCE_COUNT].RecordReference;
		(*pVector)[i / BLOCK_REFERENCE_COUNT].IndexBlock[i % BLOCK_REFERENCE_COUNT].RecordReference = reference;
		(*pVector)[reference / BLOCK_REFERENCE_COUNT].IndexBlock[reference % BLOCK_REFERENCE_COUNT].IndexReference = i;
		reference = tmp;
		if(i == oldIndex)
			break;
	}
	(*pVector)[indexReference / BLOCK_REFERENCE_COUNT].IndexBlock[indexReference % BLOCK_REFERENCE_COUNT].RecordReference = ReferenceNumber;
	(*pVector)[ReferenceNumber / BLOCK_REFERENCE_COUNT].IndexBlock[ReferenceNumber % BLOCK_REFERENCE_COUNT].IndexReference = indexReference;

	return 0;
}
*/

//************************************
// Method:    SearchRecord
// FullName:  二分法定位记录，返回记录序号
// Access:    public 
// Returns:   UINT64
// Qualifier:
// Parameter: PINDEX_RECORD pIndex
// Parameter: PVOID Param
// Parameter: PLONG compResult
//************************************
UINT64 CRecordFile::SearchRecord(PINDEX_RECORD pIndex, PVOID Param, PLONG compResult)
{
	if(!pIndex)
		return 0;

	PRECORD_POINTER rs = NULL;
	UINT64 b = 0, m;
	UINT64 e = pIndex->count;
	LONG r = 0xFFF;        
	UCHAR Record[MAX_RECORD_LENGTH] = {0};
	BOOL bHave = FALSE;
	UINT64 lastEq = 0;

	while(b < e){
		m = (b + e) / 2;
		rs = GetRecordPointer(pIndex->Index[m]);
		while(!rs || rs->Pointer == 0){    //文件可能被删除或无效
			m++;
			if(m>e){
				e = (b + e) / 2;
				break;
			}
			rs = GetRecordPointer(pIndex->Index[m]);
		}
		if(!rs || rs->Pointer == 0){  //向下没有有效的，向上找
			m = (b + e) /2;
			while(!rs || rs->Pointer == 0){
				m--;
				if(m<b){
					b = (b + e) /2;
					break;
				}
				rs = GetRecordPointer(pIndex->Index[m]);
			}
		}
		if(rs && rs->Pointer > 0){
			ReadRecord(rs, Record, rs->Length);
			r = m_Holder->EnumFileRecordCallback(pIndex->Index[m], Record, rs->Length, Param);

			if(r == 0){
				bHave = TRUE;  //not only one 
				lastEq = m;
				if(compResult && *compResult == 1){  //find the first;
					if(e == m + 1)
						break;
					e = m;
				}else {
					if(b == m -1)
						break;
					b = m;                          //find the last;
				}
			}else if(r >0){
				if(b == m - 1)
					break;
				b = m ;
			}else {
				if(e == m + 1)
					break;
				e = m;
			}

		}else
			break;
	}
	if(bHave){
		if(compResult)
			*compResult = 0;
		return lastEq;
	}else{
		if(compResult)
			*compResult = r;
		return b;
	}
}

/*
CMftFile::PRECORD_POINTER CMftFile::GetIndexRecordPointer(vector<BLOCK_RECORD>* pVector, UINT64 IndexReference)
{
	DWORD i = (DWORD)(IndexReference / BLOCK_REFERENCE_COUNT);
	DWORD j = (DWORD)(IndexReference % BLOCK_REFERENCE_COUNT);
	return GetRecordPointer((*pVector)[i].IndexBlock[j].RecordReference);
}
*/
//************************************
// Method:    GetLastReference
// FullName:  最后一个有效的记录序号
// Access:    private 
// Returns:   UINT64
// Qualifier:
//************************************
UINT64 CRecordFile::GetLastReference()
{
	for(INT64 i = m_RecordIndexBlocks.size() -1; i >= 0; i--)
		for(int j = BLOCK_REFERENCE_COUNT - 1; j >= 0; j--){
			PRECORD_POINTER ps = (PRECORD_POINTER)(File2MapPointer(m_RecordIndexBlocks[i] + j * sizeof(RECORD_POINTER)));
			if(ps->Pointer > 0 && ps->Length >0)
				return i * BLOCK_REFERENCE_COUNT + j;
		}
			
	return 0;
}

//************************************
// Method:    InitFileMapView
// FullName:  创建文件内存影像
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool CRecordFile::InitFileMapView()
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	bool result = false;

	LARGE_INTEGER fSize = {0};
	GetFileSizeEx(m_hFile, &fSize);

	if(fSize.QuadPart > 0){
		m_hFileMap = CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
		if(m_hFileMap){
			m_pMapStart = (PUCHAR)MapViewOfFile(m_hFileMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, fSize.LowPart);
			if(m_pMapStart){
				m_Header = (PFILE_HEADER)m_pMapStart;
				m_Header->AllocSize = fSize.LowPart;
				if(m_Header->FileSize == 0)
					m_Header->FileSize = sizeof(FILE_HEADER);

				result = true;
			}else{
				CloseHandle(m_hFileMap);
				m_hFileMap = NULL;
			}
		}
	}
	
	return result;
}

bool CRecordFile::IsValid()
{
	return TRUE; // m_hFile != INVALID_HANDLE_VALUE;
}

//************************************
// Method:    DeleteAllRecord
// FullName:  清空文件记录，文件长度设置为0
// Access:    public 
// Returns:   DWORD
// Qualifier:
//************************************
DWORD CRecordFile::DeleteAllRecord()
{

	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		SaveFile();
		m_RecordIndexBlocks.clear();

		LARGE_INTEGER ps = {0, 0}, newPs;
		if(!SetFilePointerEx(m_hFile, ps, &newPs, FILE_BEGIN)){
			return 0;
		}
		if(!SetEndOfFile(m_hFile)){
			return 0;
		}
	}
	else
		FreeSpace();
	
	return 1;
}

//************************************
// Method:    AllocSpace
// FullName:  在文件中分配一段未使用空间
// Access:    private 
// Returns:   DWORD
// Qualifier:
// Parameter: DWORD nSize
//************************************
UINT64 CRecordFile::AllocSpace(DWORD nSize)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		PVOID p = new BYTE[nSize];
		ZeroMemory(p, nSize);
		return (UINT64)p;
	}		
	else
	{
		if (!m_Header || nSize >= m_Header->AllocSize - m_Header->FileSize) { //如果超过文件长度则需增加文件长度（一次增加 FILE_PAGE_SIZE 字节）
			SaveFile();

			LARGE_INTEGER ps = { FILE_PAGE_SIZE, 0 }, newPs;
			if (!SetFilePointerEx(m_hFile, ps, &newPs, FILE_END)) {
				return 0;
			}
			if (!SetEndOfFile(m_hFile)) {
				return 0;
			}

			if (!InitFileMapView())
				return 0;

		}

		m_Header->FileSize += nSize;

		return m_Header->FileSize - nSize;
	}
}

VOID CRecordFile::FreeSpace()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return;

	if (m_Header)
	{
		delete m_Header;
		m_Header = nullptr;
	}
	for (int i = m_RecordIndexBlocks.size() - 1; i >= 0; i--)
	{
		for (int j = BLOCK_REFERENCE_COUNT - 1; j >= 0; j--) {
			PRECORD_POINTER ps = (PRECORD_POINTER)(File2MapPointer(m_RecordIndexBlocks[i] + j * sizeof(RECORD_POINTER)));
			if (ps->Pointer > 0)
				delete[](PBYTE)ps->Pointer;
		}
		delete[](PBYTE)m_RecordIndexBlocks[i];
	}
	m_RecordIndexBlocks.clear();
}

//************************************
// Method:    SaveFile
// FullName:  保存并关闭内存影像
// Access:    private 
// Returns:   bool
// Qualifier:
//************************************
BOOL CRecordFile::SaveFile()
{
	if(m_pMapStart){
		FlushViewOfFile(m_pMapStart, m_Header->AllocSize);
		UnmapViewOfFile(m_pMapStart);
		m_pMapStart = NULL;
		m_Header = NULL;
	}

	if(m_hFileMap){
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}	

	return true;
}

PVOID CRecordFile::File2MapPointer(UINT64 Pointer)
{
	if (m_hFile == INVALID_HANDLE_VALUE)
		return (PVOID)Pointer;
	else
	{
		if (!m_pMapStart | (Pointer > m_Header->AllocSize))
			return NULL;

		return m_pMapStart + Pointer;
	}
}

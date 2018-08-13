/*!
 * 记录文件(不关心数据内容),分块存储以适应记录长度可变。
 * 使用内存映像文件提高访问速度
 * 文件结构：
 * 1、文件头（见：MFTFILE_HEADER）记录基本信息。
 * 2、记录编号索引（结构数组，编号为数组下标）
      记录每个文件记录这转存文件的位置和长度（见：RECORD_POINTER）
      每BLOCK_REFERENCE_COUNT个为一数据块。数据块是单向链表。
 * 4、文件记录（记录长度不能超过MAX_RECORD_LENGTH字节）。
 * 5、其他信息（OptionRecord），为方便管理允许存储5个其他信息记录。
*/

#pragma once

/*
* 使用者的回掉函数。
*/
interface IRecordFileHandler
{
	//查找记录的回掉函数。
	virtual LONG EnumRecordCallback(UINT64, const PUCHAR, USHORT, PVOID) = 0; 
	//建立索引时的排序回掉函数。
	virtual LONG RecordComparer(UINT64, const PUCHAR, USHORT, UINT64, const PUCHAR, USHORT, PVOID Param) = 0; 
};

//为快速查找记录（查找重复记录）创建记录索引。
typedef struct _INDEX_RECORD{  
	UINT64 count;   //索引包含的记录个数。
	UINT64* Index;  //记录序号数组。
}INDEX_RECORD, *PINDEX_RECORD;

class CRecordFile
{
	friend int _cdecl sortCompare(void *, const void *, const void *);
private:

#define MAX_OPTION 5

#pragma pack(push, 1)

	typedef struct RECORD_POINTER{  //记录在文件中位置、长度（字节数）
		USHORT Length;       //记录字节数
		UINT64 Pointer;    //在文件中的位置
	}*PRECORD_POINTER;

	typedef struct FILE_HEADER{  //文件头
		UCHAR Flag;          
		ULONG Version;
		UINT64 AllocSize;                  //文件占磁盘空间
		UINT64 FileSize;                   //文件实际使用字节数
		UINT64 LastReferenceBlock;            //最后一索引块（BLOCK_RECORD）在文件中的偏移位置
		RECORD_POINTER OptionRecord[MAX_OPTION];      //其他信息位置
		UCHAR Reserved[100]; //保留
	}*PFILE_HEADER;
#pragma pack(pop)

public:
	CRecordFile();
	~CRecordFile(void);
	BOOL OpenFile(const TCHAR* Filename);         // 文件全名
	BOOL SaveFile();

	BOOL WriteRecord( //写文件记录
		UINT64 ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadRecord( //读文件记录
		UINT64 ReferenceNumber, PVOID Buffer, USHORT Length);
	VOID DeleteRecord( //删除文件记录
		UINT64 ReferenceNumber);
	ULONG DeleteAllRecord();        //清除所有记录

	BOOL WriteOptionRecord(//
		UCHAR ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadOptionRecord(UCHAR ReferenceNumber, PUCHAR Buffer, USHORT Length);

	UINT64 EnumRecord(             //逐个列举文件记录
		IRecordFileHandler* Holder,
		PVOID Param,                      //回掉函数参数（IRecordFileHolder::EnumFileRecordCallback）
		PINDEX_RECORD pIndex = NULL,      //使用索引（CreateIndex创建并返回的序号）
		UINT64 begin = 0,              //从索引的开始位置（BinSearch找到并返回位置以缩小范围）
		UINT64 end = 0                 //索引的结束位置
		);                                //返回记录总数

	UINT64 SearchRecord(                  //二分法查找特定项在索引中的位置
		IRecordFileHandler* Holder,
		PINDEX_RECORD pIndex,             //使用索引
		PVOID Param,                      //比较函数参数（IRecordFileHolder::EnumFileRecordCallback）
		PLONG compResult                  //最后一个比较结果（>0  ==0 <0),有可能存在多个=0的结果，也有可能没有。
		);                                //返回索引位置

	PINDEX_RECORD SortRecord(            //创建索引（索引不永久保存)
		PVOID param,                      //比较函数参数（IRecordFileHolder::RecordComparer）
		CLdArray<UINT64>* pReferences = NULL //指定记录序号
		);                                //返回索引序号

	//void UpdateIndexToFile();             //当文件内容有改变时把内存改变内容写回文件中
	bool InitFileMapView();               //读取文件后或较大改变后更新内存映像文件
	bool IsValid();                       //判断文件是否有效
	UINT64 GetLastReference();            //最后一个有效的文件序号
private:
	IRecordFileHandler* m_Holder;         //回掉函数接口（创建时传入）
	HANDLE m_hFile;                      //打开的物理文件句柄
	CLdArray<UINT64> m_RecordIndexBlocks;  //记录序号块（没一个块含BLOCK_REFERENCE_COUNT个序号）在文件中偏移位置

	PFILE_HEADER m_Header;             //文件头
	
	HANDLE m_hFileMap;                   //内存影像文件句柄
	PUCHAR m_pMapStart;                  //内存影像文件起始地址
	//UINT64 m_MapSize;                 //内存影像文件大小
	//UINT64 m_FileSize;

	VOID UpdateHeader();                 //
	BOOL LoadRecordIndex();              //读取文件中的记录索引块
	void FreeIndex();                      

	PRECORD_POINTER GetRecordPointer(UINT64 ReferenceNumber);   
	void SetRecordPointer(UINT64 ReferenceNumber, ULONG Pointer, USHORT Length);
	ULONG WriteRecord(PRECORD_POINTER rs, PUCHAR pRecord, USHORT Length);
	USHORT ReadRecord(PRECORD_POINTER rs, PVOID Buffer, USHORT Length);

	PRECORD_POINTER AddFileReference(UINT64 ReferenceNumber);       
	ULONG WriteBlockToFile(ULONG Pointer, ULONG blockSize);

	BOOL MyReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, ULONG Pointer);
	//ULONG MyWriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, ULONG Pointer);
	PVOID File2MapPointer(UINT64 Pointer);

	UINT64 EnumRecordByIndex(PVOID Param, PINDEX_RECORD pIndex, UINT64 begin, UINT64 end);
	UINT64 AllocSpace(ULONG nSize);
	VOID FreeSpace();
	bool CreateFileView();
	void CloseFileView();
};


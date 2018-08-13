/*!
 * ��¼�ļ�(��������������),�ֿ�洢����Ӧ��¼���ȿɱ䡣
 * ʹ���ڴ�ӳ���ļ���߷����ٶ�
 * �ļ��ṹ��
 * 1���ļ�ͷ������MFTFILE_HEADER����¼������Ϣ��
 * 2����¼����������ṹ���飬���Ϊ�����±꣩
      ��¼ÿ���ļ���¼��ת���ļ���λ�úͳ��ȣ�����RECORD_POINTER��
      ÿBLOCK_REFERENCE_COUNT��Ϊһ���ݿ顣���ݿ��ǵ�������
 * 4���ļ���¼����¼���Ȳ��ܳ���MAX_RECORD_LENGTH�ֽڣ���
 * 5��������Ϣ��OptionRecord����Ϊ�����������洢5��������Ϣ��¼��
*/

#pragma once

/*
* ʹ���ߵĻص�������
*/
interface IRecordFileHandler
{
	//���Ҽ�¼�Ļص�������
	virtual LONG EnumRecordCallback(UINT64, const PUCHAR, USHORT, PVOID) = 0; 
	//��������ʱ������ص�������
	virtual LONG RecordComparer(UINT64, const PUCHAR, USHORT, UINT64, const PUCHAR, USHORT, PVOID Param) = 0; 
};

//Ϊ���ٲ��Ҽ�¼�������ظ���¼��������¼������
typedef struct _INDEX_RECORD{  
	UINT64 count;   //���������ļ�¼������
	UINT64* Index;  //��¼������顣
}INDEX_RECORD, *PINDEX_RECORD;

class CRecordFile
{
	friend int _cdecl sortCompare(void *, const void *, const void *);
private:

#define MAX_OPTION 5

#pragma pack(push, 1)

	typedef struct RECORD_POINTER{  //��¼���ļ���λ�á����ȣ��ֽ�����
		USHORT Length;       //��¼�ֽ���
		UINT64 Pointer;    //���ļ��е�λ��
	}*PRECORD_POINTER;

	typedef struct FILE_HEADER{  //�ļ�ͷ
		UCHAR Flag;          
		ULONG Version;
		UINT64 AllocSize;                  //�ļ�ռ���̿ռ�
		UINT64 FileSize;                   //�ļ�ʵ��ʹ���ֽ���
		UINT64 LastReferenceBlock;            //���һ�����飨BLOCK_RECORD�����ļ��е�ƫ��λ��
		RECORD_POINTER OptionRecord[MAX_OPTION];      //������Ϣλ��
		UCHAR Reserved[100]; //����
	}*PFILE_HEADER;
#pragma pack(pop)

public:
	CRecordFile();
	~CRecordFile(void);
	BOOL OpenFile(const TCHAR* Filename);         // �ļ�ȫ��
	BOOL SaveFile();

	BOOL WriteRecord( //д�ļ���¼
		UINT64 ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadRecord( //���ļ���¼
		UINT64 ReferenceNumber, PVOID Buffer, USHORT Length);
	VOID DeleteRecord( //ɾ���ļ���¼
		UINT64 ReferenceNumber);
	ULONG DeleteAllRecord();        //������м�¼

	BOOL WriteOptionRecord(//
		UCHAR ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadOptionRecord(UCHAR ReferenceNumber, PUCHAR Buffer, USHORT Length);

	UINT64 EnumRecord(             //����о��ļ���¼
		IRecordFileHandler* Holder,
		PVOID Param,                      //�ص�����������IRecordFileHolder::EnumFileRecordCallback��
		PINDEX_RECORD pIndex = NULL,      //ʹ��������CreateIndex���������ص���ţ�
		UINT64 begin = 0,              //�������Ŀ�ʼλ�ã�BinSearch�ҵ�������λ������С��Χ��
		UINT64 end = 0                 //�����Ľ���λ��
		);                                //���ؼ�¼����

	UINT64 SearchRecord(                  //���ַ������ض����������е�λ��
		IRecordFileHandler* Holder,
		PINDEX_RECORD pIndex,             //ʹ������
		PVOID Param,                      //�ȽϺ���������IRecordFileHolder::EnumFileRecordCallback��
		PLONG compResult                  //���һ���ȽϽ����>0  ==0 <0),�п��ܴ��ڶ��=0�Ľ����Ҳ�п���û�С�
		);                                //��������λ��

	PINDEX_RECORD SortRecord(            //�������������������ñ���)
		PVOID param,                      //�ȽϺ���������IRecordFileHolder::RecordComparer��
		CLdArray<UINT64>* pReferences = NULL //ָ����¼���
		);                                //�����������

	//void UpdateIndexToFile();             //���ļ������иı�ʱ���ڴ�ı�����д���ļ���
	bool InitFileMapView();               //��ȡ�ļ����ϴ�ı������ڴ�ӳ���ļ�
	bool IsValid();                       //�ж��ļ��Ƿ���Ч
	UINT64 GetLastReference();            //���һ����Ч���ļ����
private:
	IRecordFileHandler* m_Holder;         //�ص������ӿڣ�����ʱ���룩
	HANDLE m_hFile;                      //�򿪵������ļ����
	CLdArray<UINT64> m_RecordIndexBlocks;  //��¼��ſ飨ûһ���麬BLOCK_REFERENCE_COUNT����ţ����ļ���ƫ��λ��

	PFILE_HEADER m_Header;             //�ļ�ͷ
	
	HANDLE m_hFileMap;                   //�ڴ�Ӱ���ļ����
	PUCHAR m_pMapStart;                  //�ڴ�Ӱ���ļ���ʼ��ַ
	//UINT64 m_MapSize;                 //�ڴ�Ӱ���ļ���С
	//UINT64 m_FileSize;

	VOID UpdateHeader();                 //
	BOOL LoadRecordIndex();              //��ȡ�ļ��еļ�¼������
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


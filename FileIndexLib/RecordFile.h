/*!
 * ��¼�ļ�(��������������),�ֿ�洢����Ӧ��¼���ȿɱ䡣
 * ʹ���ڴ�ӳ���ļ���߷����ٶ�
 * �ļ��ṹ��
 * 1���ļ�ͷ������MFTFILE_HEADER����¼������Ϣ��
 * 2����¼����������ṹ���飬���Ϊ�����±꣩
      ��¼ÿ���ļ���¼��ת���ļ���λ�úͳ��ȣ�����RECORD_POINTER��
      ÿBLOCK_REFERENCE_COUNT��Ϊһ���ݿ顣���ݿ��ǵ�������
 * 4���ļ���¼����¼���Ȳ��ܳ���1024�ֽڣ���
 * 5��������Ϣ��OptionRecord����Ϊ�����������洢5��������Ϣ��¼��
*/

#pragma once

#include "stdafx.h"

#define MAX_RECORD_LENGTH 1024
#define BLOCK_REFERENCE_COUNT  1024

/*
* ʹ���ߵĻص�������
*/
typedef struct IRecordFileHolder{
	//���Ҽ�¼�Ļص�������
	virtual LONG EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param) = 0; 
	//��������ʱ������ص�������
	virtual LONG RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param) = 0; 
}*PRecordFileHolder;

//Ϊ���ٲ��Ҽ�¼�������ظ���¼��������¼������
typedef struct _INDEX_RECORD{  
	ULONGLONG count;   //���������ļ�¼������
	ULONGLONG* Index;  //��¼������顣
}INDEX_RECORD, *PINDEX_RECORD;

class CRecordFile
{
	friend int sortCompare(void *, const void *, const void *);
private:

#define MAX_OPTION 5

#pragma pack(push, 1)

	typedef struct RECORD_POINTER{  //��¼���ļ���λ�á����ȣ��ֽ�����
		USHORT Length;       //��¼�ֽ���
		ULONG Pointer;    //���ļ��е�λ��
	}*PRECORD_POINTER;

	//typedef struct RECORD_DATA{  //��¼����
		//bool Deleted;     //�Ƿ�ɾ��      
		//USHORT Length;       //��¼�ֽ���
		//UCHAR Data[0];    
	//}*PRECORD_DATA;

	//typedef struct BLOCK_RECORD{  //��¼�����������ݿ�
		//ULONG Pointer;         //���ļ��е�λ��
		//ULONG PrvPointer;      //ǰһ�������ݿ����ļ��е�λ�ã��ļ�ͷ�м�¼���һ���������λ�ã�
		//RECORD_POINTER RecordBlock[BLOCK_REFERENCE_COUNT];  //��¼�������飨����ΪBLOCK_REFERENCE_COUNT��
	//}*PBLOCK_RECORD;

	typedef struct FILE_HEADER{  //�ļ�ͷ
		UCHAR Flag;          
		ULONG Version;
		ULONG AllocSize;                  //�ļ�ռ���̿ռ�
		ULONG FileSize;                   //�ļ�ʵ��ʹ���ֽ���
		ULONG LastReferenceBlock;            //���һ�����飨BLOCK_RECORD�����ļ��е�ƫ��λ��
		RECORD_POINTER OptionRecord[MAX_OPTION];      //������Ϣλ��
		UCHAR Reserved[100]; //����
	}*PFILE_HEADER;
#pragma pack(pop)

public:
	CRecordFile(PRecordFileHolder Holder);
	~CRecordFile(void);
	BOOL OpenFile(LPCTSTR Filename);         // �ļ�ȫ��
	BOOL SaveFile();

	BOOL WriteRecord( //д�ļ���¼
		ULONGLONG ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadRecord( //���ļ���¼
		ULONGLONG ReferenceNumber, PVOID Buffer, USHORT Length);
	VOID DeleteRecord( //ɾ���ļ���¼
		ULONGLONG ReferenceNumber);
	ULONG DeleteAllRecord();        //������м�¼

	BOOL WriteOptionRecord(//
		UCHAR ReferenceNumber, PUCHAR pRecord, USHORT Length);
	USHORT ReadOptionRecord(UCHAR ReferenceNumber, PUCHAR Buffer, USHORT Length);

	ULONGLONG EnumRecord(             //����о��ļ���¼
		PVOID Param,                      //�ص�����������IRecordFileHolder::EnumFileRecordCallback��
		PINDEX_RECORD pIndex = NULL,      //ʹ��������CreateIndex���������ص���ţ�
		ULONGLONG begin = 0,              //�������Ŀ�ʼλ�ã�BinSearch�ҵ�������λ������С��Χ��
		ULONGLONG end = 0                 //�����Ľ���λ��
		);                                //���ؼ�¼����

	ULONGLONG SearchRecord(                  //���ַ������ض����������е�λ��
		PINDEX_RECORD pIndex,             //ʹ������
		PVOID Param,                      //�ȽϺ���������IRecordFileHolder::EnumFileRecordCallback��
		PLONG compResult                  //���һ���ȽϽ����>0  ==0 <0),�п��ܴ��ڶ��=0�Ľ����Ҳ�п���û�С�
		);                                //��������λ��

	PINDEX_RECORD SortRecord(            //�������������������ñ���)
		PVOID param,                      //�ȽϺ���������IRecordFileHolder::RecordComparer��
		vector<ULONGLONG>* pReferences = NULL //ָ����¼���
		);                                //�����������

	//void UpdateIndexToFile();             //���ļ������иı�ʱ���ڴ�ı�����д���ļ���
	bool InitFileMapView();               //��ȡ�ļ����ϴ�ı������ڴ�ӳ���ļ�
	bool IsValid();                       //�ж��ļ��Ƿ���Ч

private:
	PRecordFileHolder m_Holder;         //�ص������ӿڣ�����ʱ���룩
	HANDLE m_hFile;                      //�򿪵������ļ����
	vector<ULONG> m_RecordIndexBlocks;  //�ļ�������

	PFILE_HEADER m_Header;             //�ļ�ͷ
	
	HANDLE m_hFileMap;                   //�ڴ�Ӱ���ļ����
	PUCHAR m_pMapStart;                  //�ڴ�Ӱ���ļ���ʼ��ַ
	//ULONGLONG m_MapSize;                 //�ڴ�Ӱ���ļ���С
	//ULONGLONG m_FileSize;

	VOID UpdateHeader();                 //
	BOOL Init();                         //�򿪡���ȡ�ļ��������ڴ�Ӱ���ļ�
	BOOL LoadRecordIndex();              //��ȡ�ļ��еļ�¼������
	void FreeIndex();                      

	PRECORD_POINTER GetRecordPointer(ULONGLONG ReferenceNumber);   
	void SetRecordPointer(ULONGLONG ReferenceNumber, ULONG Pointer, USHORT Length);
	ULONG WriteRecord(PRECORD_POINTER rs, PUCHAR pRecord, USHORT Length);
	USHORT ReadRecord(PRECORD_POINTER rs, PVOID Buffer, USHORT Length);

	PRECORD_POINTER AddFileReference(ULONGLONG ReferenceNumber);       
	ULONG WriteBlockToFile(ULONG Pointer, ULONG blockSize);

	BOOL MyReadFile(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, ULONG Pointer);
	//ULONG MyWriteFile(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, ULONG Pointer);
	PVOID File2MapPointer(ULONG Pointer);
	ULONGLONG GetLastReference();  //���һ����Ч���ļ����

	ULONGLONG EnumRecordByIndex(PVOID Param, PINDEX_RECORD pIndex, ULONGLONG begin, ULONGLONG end);
	ULONG AllocSpace(ULONG nSize);
};


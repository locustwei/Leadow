/*!
 * ���̷�����Volume����
 * 1������������Ϣ��Guid��Path����
 * 2����ȡ�ļ�����������ļ������ת���ļ���
 * 3������UsnJoural�����ļ��䶯ʵʱ����ת���ġ�
 * 4����ת���ļ��н���������MD5��
 * 5��ʹ���ļ�������ͨ��������ļ���С�������ļ���
 * 6������ͬ����ͬMD5�ļ�
*/

#pragma once

#include "stdafx.h"
#include "MftReader.h"
#include "RecordFile.h"
#include "Thread.h"
#include "MD5.h"

/*
* ת���ļ���¼����
*/
typedef enum RECORD_FIELD_CLASS{
	RFC_NONE =  0x00000000,
	RFC_FN =    0x00000001,  //FileReferenceNumber
	RFC_DIR =   0x00000002,  //IsDir
	RFC_DRN =   0x00000004,  //DirectoryFileReferenceNumber
	RFC_CRT =   0x00000008,  //CreationTime
	RFC_CHT =   0x00000010,  //ChangeTime
	RFC_LWT =   0x00000020,  //LastWriteTime
	RFC_LAT =   0x00000040,  //LastAccessTime
	RFC_DS =    0x00000080,  //DataSize
	RFC_NL =    0x00000100,  //NameLength
	RFC_MD5 =   0x00000200,  //MD5
	RFC_NAME =  0x00000400   //FileName
};

typedef struct FILTER_DIRECTORY{
	BOOL sub;
	ULONGLONG DircetoryReference;
}*PFILTER_DIRECTORY;

/*
* �����ļ�����Χ��������
*/
typedef struct FILE_FILTER{
	PCWSTR* pIncludeNames;                           //�ļ������� 
	PCWSTR* pExcludeNames;                           //�ļ���������
	PFILTER_DIRECTORY* ParentDircetoryReferences;    //����Ŀ¼
	LONGLONG beginSize;                              //�ļ���С >=
	LONGLONG endSize;                                //�ļ���С <=
	BOOL onlyFile;                                   //����Ҫ�ļ�
}*PFILE_FILTER;

typedef struct ENUM_FILERECORD_PARAM{
	RECORD_FIELD_CLASS* sortField;                   //�������
	FILE_FILTER Filter;
}*PENUM_FILERECORD_PARAM;

#pragma pack(push, 1)

typedef struct MFTFILERECORD{                //ת���ļ���¼�ṹ
	//ULONGLONG FileReferenceNumber;         //0x00000001
	bool IsDir;                              //0x00000002
// 	union{
// 		struct{
// 			USHORT nVolume;
// 			ULONGLONG DirectoryReferenceNumber :48;
// 		};
// 		ULONGLONG DirectoryFileReferenceNumber;  //0x00000004
// 	};
	ULONGLONG DirectoryFileReferenceNumber;
	//	ULONGLONG CreationTime;              //0x00000008 
	//	ULONGLONG ChangeTime;                //0x00000010
	//	ULONGLONG LastWriteTime;             //0x00000020
	//	ULONGLONG LastAccessTime;            //0x00000040
	ULONGLONG DataSize;                      //0x00000080
	UCHAR NameLength;                        //0x00000100
	BYTE MD5[MD5_CODE_LEN];                  //0x00000200
	WCHAR Name[MAX_PATH + 1];                //0x00000400
}*PMFTFILERECORD;

#pragma pack(pop)

#pragma warning( disable : 4091 )
//typedef BOOL (*VolumesGuidEnumCallback)(const PWSTR wsz_guid, PVOID Param); 
typedef struct IVolumeInterface;

//ʹ���߻ص��ӿ�
typedef struct IDiskVolumeHolder{
	virtual VOID __stdcall OnSearchCompeleted(IVolumeInterface*, vector<PMFTFILERECORD>*) = 0;
	virtual VOID __stdcall OnRenewDump(IVolumeInterface*) = 0;
	virtual VOID __stdcall OnFindDupCompeleted(IVolumeInterface*, vector<vector<PMFTFILERECORD> *>*) = 0;
}*PDiskVolumeHolder;

//���ڶ�̬�⵼���ӿڡ�
typedef struct IVolumeInterface{
	virtual BOOL __stdcall OpenVolume(PWSTR) = 0;
	virtual PCWSTR __stdcall GetVolumeGuid() = 0;
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume) = 0;
	virtual PCWSTR __stdcall GetVolumePath() = 0;
	virtual BOOL __stdcall LoadDumpFile(PCWSTR wsz_path) = 0; 
	virtual BOOL __stdcall UnLoadDumpFile() = 0; 
	virtual PCWSTR __stdcall GetDumpFileName() = 0;
	virtual VOID __stdcall SetHolper(PDiskVolumeHolder pHoler) = 0;

	virtual BOOL __stdcall UpdateMftDump(BOOL async) = 0;
	virtual BOOL __stdcall ListenFileChange() = 0;

	virtual vector<vector<PMFTFILERECORD> *>* __stdcall FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn) = 0;
	virtual vector<PMFTFILERECORD>* __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn) = 0;
}*PVolumeInterface;

class FIL_API CDiskVolume: IVolumeInterface, IMftReadeHolder, IRecordFileHolder,IRunable
{
public:
	CDiskVolume(void);
	~CDiskVolume(void);

	virtual BOOL __stdcall OpenVolume(PWSTR wsz_guid);         //�þ�GUID���򿪾���
	virtual PCWSTR __stdcall GetVolumeGuid();
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume); //��·�����̷������򿪾���
	virtual PCWSTR __stdcall GetVolumePath();
	virtual BOOL __stdcall LoadDumpFile(PCWSTR wsz_path);      //����ת���ļ�
	virtual BOOL __stdcall UnLoadDumpFile();                    //�ͷ�ת���ļ�
	virtual PCWSTR __stdcall GetDumpFileName();
	virtual VOID __stdcall SetHolper(PDiskVolumeHolder pHoler);

	virtual BOOL __stdcall UpdateMftDump(BOOL async);       //�����ļ������ת���ļ�������Ѵ�������¡�
	virtual BOOL __stdcall ListenFileChange();               //�����̶߳�ʱ�����ļ���
		
	virtual vector<vector<PMFTFILERECORD> *>* __stdcall FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //������ͬ�ļ���ͬ������С��MD5��
	virtual vector<PMFTFILERECORD>* __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn);                             //�����ļ�
	ULONG GetFullFileName(ULONGLONG FileReferenceNumber, PWSTR wsz_Directory, ULONG nameLength);
private:

#pragma pack(push, 1)
	typedef struct VOLUME_MFT_INFO{
		USN LastUsn;
		ULONGLONG FileCount;
		//BOOL Md5ed;
	};
#pragma pack(pop)

	typedef struct INDEX_RECORD{
		PINDEX_RECORD Index;
		RECORD_FIELD_CLASS* c;
	};

	stringxw m_VolumeGuid;           //��GUID
	stringxw m_VolumePath;           //·��
	stringxw m_MftDumpFileName;      //�ļ������ת���ļ�
	vector<INDEX_RECORD> m_Indexs;   //���������������ļ�ʹ������

	HANDLE m_hVolume;                //��������ȡMFT��
	CMftReader* m_MftReader;     //MFT��������
	CRecordFile* m_MftFile;          //ת���ļ�����
	VOLUME_MFT_INFO m_MftInfo;                           //��ĳ־���Ϣ��������ת���ļ��У�
	PDiskVolumeHolder m_Holder;                          //�ص��ӿ�

	//vector<vector<PMFTFILERECORD> *> DuplicateResult;   //�����ظ��ļ�����ʱʹ�á�
	//vector<ULONGLONG> m_FilterResult;                    //��������ǰ�ȹ��˳����������ļ�¼

	BOOL CreateMftDump();                                 //ɾ�����м�¼��������ת���ļ�    
	BOOL UpdateFiles();                             //����USNJoural����ת���ļ�
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile����hVolume
	BOOL MakeFileMd5(ULONGLONG ReferenceNumber, BYTE* Md5Code);  //�����ļ�MD5

	CRITICAL_SECTION m_cs;                                  //�߳��ٽ�������ȡת���ļ�ʱ���⣩ 
	inline void lock(){EnterCriticalSection(&m_cs);}
	inline void unlock(){LeaveCriticalSection(&m_cs);}

	BOOL m_ListenThreadTerminated;

	//CThread* m_ListenFileChangeThread;                     //�����ļ�����߳�
	VOID RunListenFileChange();                            //�����ļ��޸��̺߳���
	BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, vector<vector<PMFTFILERECORD> *>* pResult);  //������ͬ�ļ���ͬ������С��MD5��
	BOOL SearchFile(PFILE_FILTER pFilter, vector<PMFTFILERECORD>* Result);

	//LONG CompareFileData(ULONGLONG ReferenceNumber1, ULONGLONG ReferenceNumber2, ULONGLONG FileLength, BYTE* md5Code1, BYTE* md5Code2);
	BOOL FilterRecord(PFILE_FILTER pFilter, PMFTFILERECORD pRecord);  
	LONG CompareFileRecord(PMFTFILERECORD pSource, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord);
	//void CleanDuplicateFiles();
	LONG DoDuplicateFile(vector<vector<PMFTFILERECORD> *>* pFiles, PMFTFILERECORD pPrv, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord, USHORT Length);

	//ULONG GetFullFileName(ULONGLONG FileReferenceNumber, PWSTR wsz_Directory, ULONG cb);
	PINDEX_RECORD CreateIndex(RECORD_FIELD_CLASS* c, PFILE_FILTER pFilter);
	BOOL FullName2ReferenceNumber(PWSTR wsFullName, PULONGLONG ReferenceNumber);

private:  //�ӿں���
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(WPARAM Param);

	virtual VOID OnThreadInit(WPARAM Param);

	virtual VOID OnThreadTerminated(WPARAM Param);

};

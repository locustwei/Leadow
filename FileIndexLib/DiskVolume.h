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
#include "StructDefines.h"
#include "ExportFunction.h"

class CDiskVolume: public IVolumeInterface, IMftReadeHolder, IRecordFileHolder,IThreadRunable
{
public:
	CDiskVolume(void);
	~CDiskVolume(void);

	virtual BOOL __stdcall OpenVolume(PWSTR wsz_guid);         //�þ�GUID���򿪾���
	virtual PCWSTR __stdcall GetVolumeGuid();
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume); //��·�����̷������򿪾���
	virtual PCWSTR __stdcall GetVolumePath();
	virtual BOOL __stdcall SetDumpFilePath(PCWSTR wsz_path);      //����ת���ļ�
	virtual BOOL __stdcall UnLoadDumpFile();                    //�ͷ�ת���ļ�
	virtual PCWSTR __stdcall GetDumpFileName();
	virtual VOID __stdcall SetHolper(IVolumeEvent* pHoler);

	virtual BOOL __stdcall UpdateMftDump(BOOL async);       //�����ļ������ת���ļ�������Ѵ�������¡�
	virtual BOOL __stdcall ListenFileChange();               //�����̶߳�ʱ�����ļ���
		
	//virtual BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //������ͬ�ļ���ͬ������С��MD5��
	virtual BOOL __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn);                             //�����ļ�
	ULONG __stdcall GetFullFileName(ULONGLONG FileReferenceNumber, PWSTR wsz_Directory, ULONG nameLength);
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
	IVolumeEvent* m_Holder;                          //�ص��ӿ�
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile����hVolume

	BOOL CreateMftDump();                                 //ɾ�����м�¼��������ת���ļ�    
	BOOL UpdateFiles();                             //����USNJoural����ת���ļ�
	BOOL MakeFileMd5(ULONGLONG ReferenceNumber, BYTE* Md5Code);  //�����ļ�MD5

	CRITICAL_SECTION m_cs;                                  //�߳��ٽ�������ȡת���ļ�ʱ���⣩ 
	inline void lock(){EnterCriticalSection(&m_cs);}
	inline void unlock(){LeaveCriticalSection(&m_cs);}

	CThread m_DumpThread;
	CThread m_ListenThread;
	CThread m_SearchThread;

	VOID RunListenFileChange();                            //�����ļ��޸��̺߳���
	BOOL SearchFile(PFILE_FILTER pFilter);

	BOOL FilterRecord(PFILE_FILTER pFilter, PMFTFILERECORD pRecord);  
	LONG CompareFileRecord(PMFTFILERECORD pSource, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord);

	PINDEX_RECORD CreateIndex(RECORD_FIELD_CLASS* c, PFILE_FILTER pFilter);
	BOOL FullName2ReferenceNumber(PWSTR wsFullName, PULONGLONG ReferenceNumber);

private:  //�ӿں���
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param);

};

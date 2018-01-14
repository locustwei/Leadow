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

	BOOL  OpenVolume(PWSTR wsz_guid) override;         //�þ�GUID���򿪾���
	PCWSTR  GetVolumeGuid() override;
	BOOL  OpenVolumePath(const PWSTR wsz_volume) override; //��·�����̷������򿪾���
	PCWSTR  GetVolumePath() override;
	BOOL  SetDumpFilePath(PCWSTR wsz_path) override;      //����ת���ļ�
	BOOL  UnLoadDumpFile() override;                    //�ͷ�ת���ļ�
	virtual PCWSTR  GetDumpFileName();
	VOID  SetHolper(IVolumeEvent* pHoler) override;

	virtual BOOL  UpdateMftDump(BOOL async);       //�����ļ������ת���ļ�������Ѵ�������¡�
	virtual BOOL  ListenFileChange();               //�����̶߳�ʱ�����ļ���
		
	//virtual BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //������ͬ�ļ���ͬ������С��MD5��
	BOOL  SearchFile(PFILE_FILTER pFilter, BOOL asyn) override;                             //�����ļ�
	DWORD  GetFullFileName(UINT64 FileReferenceNumber, PWSTR wsz_Directory, DWORD nameLength);
private:

#pragma pack(push, 1)
	typedef struct VOLUME_MFT_INFO{
		USN LastUsn;
		UINT64 FileCount;
		//BOOL Md5ed;
	};
#pragma pack(pop)

	typedef struct INDEX_RECORD{
		PINDEX_RECORD Index;
		RECORD_FIELD_CLASS* c;
	};

	CLdString m_VolumeGuid;           //��GUID
	CLdString m_VolumePath;           //·��
	CLdString m_MftDumpFileName;      //�ļ������ת���ļ�
	CLdArray<INDEX_RECORD> m_Indexs;   //���������������ļ�ʹ������

	HANDLE m_hVolume;                //��������ȡMFT��
	CMftReader* m_MftReader;     //MFT��������
	CRecordFile* m_MftFile;          //ת���ļ�����
	VOLUME_MFT_INFO m_MftInfo;                           //��ĳ־���Ϣ��������ת���ļ��У�
	IVolumeEvent* m_Holder;                          //�ص��ӿ�
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile����hVolume

	BOOL CreateMftDump();                                 //ɾ�����м�¼��������ת���ļ�    
	BOOL UpdateFiles();                             //����USNJoural����ת���ļ�
	BOOL MakeFileMd5(UINT64 ReferenceNumber, BYTE* Md5Code);  //�����ļ�MD5

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
	virtual BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(UINT64 ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(UINT64 ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, UINT64 ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param);

};

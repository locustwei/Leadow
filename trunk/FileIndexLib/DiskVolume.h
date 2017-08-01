/*!
 * 磁盘分区（Volume）。
 * 1、分区基本信息（Guid、Path）。
 * 2、读取文件分配表，生成文件分配表转存文件。
 * 3、利用UsnJoural监视文件变动实时更新转存文。
 * 4、在转存文件中建立索引、MD5。
 * 5、使用文件名（带通配符）、文件大小等搜索文件。
 * 6、搜索同名、同MD5文件
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

	BOOL  OpenVolume(PWSTR wsz_guid) override;         //用卷GUID，打开卷句柄
	PCWSTR  GetVolumeGuid() override;
	BOOL  OpenVolumePath(const PWSTR wsz_volume) override; //卷路径（盘符），打开卷句柄
	PCWSTR  GetVolumePath() override;
	BOOL  SetDumpFilePath(PCWSTR wsz_path) override;      //加载转存文件
	BOOL  UnLoadDumpFile() override;                    //释放转存文件
	virtual PCWSTR  GetDumpFileName();
	VOID  SetHolper(IVolumeEvent* pHoler) override;

	virtual BOOL  UpdateMftDump(BOOL async);       //生成文件分配表转存文件，如果已存在则更新。
	virtual BOOL  ListenFileChange();               //启动线程定时更新文件。
		
	//virtual BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //搜索相同文件（同名、大小、MD5）
	BOOL  SearchFile(PFILE_FILTER pFilter, BOOL asyn) override;                             //搜索文件
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

	CLdString m_VolumeGuid;           //卷GUID
	CLdString m_VolumePath;           //路径
	CLdString m_MftDumpFileName;      //文件分配表转存文件
	CLdArray<INDEX_RECORD> m_Indexs;   //排序索引（搜索文件使创建）

	HANDLE m_hVolume;                //卷句柄（读取MFT）
	CMftReader* m_MftReader;     //MFT分析对象。
	CRecordFile* m_MftFile;          //转存文件对象
	VOLUME_MFT_INFO m_MftInfo;                           //卷的持久信息（保存在转存文件中）
	IVolumeEvent* m_Holder;                          //回掉接口
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile，打开hVolume

	BOOL CreateMftDump();                                 //删除已有记录重新生成转存文件    
	BOOL UpdateFiles();                             //根据USNJoural更新转存文件
	BOOL MakeFileMd5(UINT64 ReferenceNumber, BYTE* Md5Code);  //生成文件MD5

	CRITICAL_SECTION m_cs;                                  //线程临界区（读取转存文件时互斥） 
	inline void lock(){EnterCriticalSection(&m_cs);}
	inline void unlock(){LeaveCriticalSection(&m_cs);}

	CThread m_DumpThread;
	CThread m_ListenThread;
	CThread m_SearchThread;

	VOID RunListenFileChange();                            //监听文件修改线程函数
	BOOL SearchFile(PFILE_FILTER pFilter);

	BOOL FilterRecord(PFILE_FILTER pFilter, PMFTFILERECORD pRecord);  
	LONG CompareFileRecord(PMFTFILERECORD pSource, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord);

	PINDEX_RECORD CreateIndex(RECORD_FIELD_CLASS* c, PFILE_FILTER pFilter);
	BOOL FullName2ReferenceNumber(PWSTR wsFullName, PULONGLONG ReferenceNumber);

private:  //接口函数
	virtual BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(UINT64 ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(UINT64 ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, UINT64 ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param);

};

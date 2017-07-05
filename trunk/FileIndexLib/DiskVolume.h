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

	virtual BOOL __stdcall OpenVolume(PWSTR wsz_guid);         //用卷GUID，打开卷句柄
	virtual PCWSTR __stdcall GetVolumeGuid();
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume); //卷路径（盘符），打开卷句柄
	virtual PCWSTR __stdcall GetVolumePath();
	virtual BOOL __stdcall SetDumpFilePath(PCWSTR wsz_path);      //加载转存文件
	virtual BOOL __stdcall UnLoadDumpFile();                    //释放转存文件
	virtual PCWSTR __stdcall GetDumpFileName();
	virtual VOID __stdcall SetHolper(IVolumeEvent* pHoler);

	virtual BOOL __stdcall UpdateMftDump(BOOL async);       //生成文件分配表转存文件，如果已存在则更新。
	virtual BOOL __stdcall ListenFileChange();               //启动线程定时更新文件。
		
	//virtual BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //搜索相同文件（同名、大小、MD5）
	virtual BOOL __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn);                             //搜索文件
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

	stringxw m_VolumeGuid;           //卷GUID
	stringxw m_VolumePath;           //路径
	stringxw m_MftDumpFileName;      //文件分配表转存文件
	vector<INDEX_RECORD> m_Indexs;   //排序索引（搜索文件使创建）

	HANDLE m_hVolume;                //卷句柄（读取MFT）
	CMftReader* m_MftReader;     //MFT分析对象。
	CRecordFile* m_MftFile;          //转存文件对象
	VOLUME_MFT_INFO m_MftInfo;                           //卷的持久信息（保存在转存文件中）
	IVolumeEvent* m_Holder;                          //回掉接口
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile，打开hVolume

	BOOL CreateMftDump();                                 //删除已有记录重新生成转存文件    
	BOOL UpdateFiles();                             //根据USNJoural更新转存文件
	BOOL MakeFileMd5(ULONGLONG ReferenceNumber, BYTE* Md5Code);  //生成文件MD5

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
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param);

	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param);

};

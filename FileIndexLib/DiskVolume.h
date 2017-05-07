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

/*
* 转存文件记录的域。
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
* 搜索文件，范围过滤条件
*/
typedef struct FILE_FILTER{
	PCWSTR* pIncludeNames;                           //文件名包含 
	PCWSTR* pExcludeNames;                           //文件名不包含
	PFILTER_DIRECTORY* ParentDircetoryReferences;    //所属目录
	LONGLONG beginSize;                              //文件大小 >=
	LONGLONG endSize;                                //文件大小 <=
	BOOL onlyFile;                                   //仅需要文件
}*PFILE_FILTER;

typedef struct ENUM_FILERECORD_PARAM{
	RECORD_FIELD_CLASS* sortField;                   //排序规则
	FILE_FILTER Filter;
}*PENUM_FILERECORD_PARAM;

#pragma pack(push, 1)

typedef struct MFTFILERECORD{                //转存文件记录结构
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

//使用者回掉接口
typedef struct IDiskVolumeHolder{
	virtual VOID __stdcall OnSearchCompeleted(IVolumeInterface*, vector<PMFTFILERECORD>*) = 0;
	virtual VOID __stdcall OnRenewDump(IVolumeInterface*) = 0;
	virtual VOID __stdcall OnFindDupCompeleted(IVolumeInterface*, vector<vector<PMFTFILERECORD> *>*) = 0;
}*PDiskVolumeHolder;

//用于动态库导出接口。
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

	virtual BOOL __stdcall OpenVolume(PWSTR wsz_guid);         //用卷GUID，打开卷句柄
	virtual PCWSTR __stdcall GetVolumeGuid();
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume); //卷路径（盘符），打开卷句柄
	virtual PCWSTR __stdcall GetVolumePath();
	virtual BOOL __stdcall LoadDumpFile(PCWSTR wsz_path);      //加载转存文件
	virtual BOOL __stdcall UnLoadDumpFile();                    //释放转存文件
	virtual PCWSTR __stdcall GetDumpFileName();
	virtual VOID __stdcall SetHolper(PDiskVolumeHolder pHoler);

	virtual BOOL __stdcall UpdateMftDump(BOOL async);       //生成文件分配表转存文件，如果已存在则更新。
	virtual BOOL __stdcall ListenFileChange();               //启动线程定时更新文件。
		
	virtual vector<vector<PMFTFILERECORD> *>* __stdcall FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, BOOL asyn);  //搜索相同文件（同名、大小、MD5）
	virtual vector<PMFTFILERECORD>* __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn);                             //搜索文件
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

	stringxw m_VolumeGuid;           //卷GUID
	stringxw m_VolumePath;           //路径
	stringxw m_MftDumpFileName;      //文件分配表转存文件
	vector<INDEX_RECORD> m_Indexs;   //排序索引（搜索文件使创建）

	HANDLE m_hVolume;                //卷句柄（读取MFT）
	CMftReader* m_MftReader;     //MFT分析对象。
	CRecordFile* m_MftFile;          //转存文件对象
	VOLUME_MFT_INFO m_MftInfo;                           //卷的持久信息（保存在转存文件中）
	PDiskVolumeHolder m_Holder;                          //回掉接口

	//vector<vector<PMFTFILERECORD> *> DuplicateResult;   //搜索重复文件是临时使用。
	//vector<ULONGLONG> m_FilterResult;                    //创建索引前先过滤出符合条件的记录

	BOOL CreateMftDump();                                 //删除已有记录重新生成转存文件    
	BOOL UpdateFiles();                             //根据USNJoural更新转存文件
	BOOL OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile，打开hVolume
	BOOL MakeFileMd5(ULONGLONG ReferenceNumber, BYTE* Md5Code);  //生成文件MD5

	CRITICAL_SECTION m_cs;                                  //线程临界区（读取转存文件时互斥） 
	inline void lock(){EnterCriticalSection(&m_cs);}
	inline void unlock(){LeaveCriticalSection(&m_cs);}

	BOOL m_ListenThreadTerminated;

	//CThread* m_ListenFileChangeThread;                     //监视文件变得线程
	VOID RunListenFileChange();                            //监听文件修改线程函数
	BOOL FindDuplicateFiles(PENUM_FILERECORD_PARAM pParam, vector<vector<PMFTFILERECORD> *>* pResult);  //搜索相同文件（同名、大小、MD5）
	BOOL SearchFile(PFILE_FILTER pFilter, vector<PMFTFILERECORD>* Result);

	//LONG CompareFileData(ULONGLONG ReferenceNumber1, ULONGLONG ReferenceNumber2, ULONGLONG FileLength, BYTE* md5Code1, BYTE* md5Code2);
	BOOL FilterRecord(PFILE_FILTER pFilter, PMFTFILERECORD pRecord);  
	LONG CompareFileRecord(PMFTFILERECORD pSource, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord);
	//void CleanDuplicateFiles();
	LONG DoDuplicateFile(vector<vector<PMFTFILERECORD> *>* pFiles, PMFTFILERECORD pPrv, RECORD_FIELD_CLASS* c, PMFTFILERECORD pRecord, USHORT Length);

	//ULONG GetFullFileName(ULONGLONG FileReferenceNumber, PWSTR wsz_Directory, ULONG cb);
	PINDEX_RECORD CreateIndex(RECORD_FIELD_CLASS* c, PFILE_FILTER pFilter);
	BOOL FullName2ReferenceNumber(PWSTR wsFullName, PULONGLONG ReferenceNumber);

private:  //接口函数
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileName, PVOID Param);

	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param);

	virtual LONG EnumFileRecordCallback(ULONGLONG ReferenceNumber, const PUCHAR pRecord, USHORT Length, PVOID Param);

	virtual LONG RecordComparer(ULONGLONG ReferenceNumber1, const PUCHAR pRecord1, USHORT Length1, ULONGLONG ReferenceNumber2, const PUCHAR pRecord2, USHORT Length2, PVOID Param);

	virtual VOID ThreadRun(WPARAM Param);

	virtual VOID OnThreadInit(WPARAM Param);

	virtual VOID OnThreadTerminated(WPARAM Param);

};

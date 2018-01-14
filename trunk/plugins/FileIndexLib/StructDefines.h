#pragma once

/*
* 转存文件记录的域。
*/
typedef enum RECORD_FIELD_CLASS {
	RFC_NONE = 0x00000000,
	RFC_FN = 0x00000001,  //FileReferenceNumber
	RFC_DIR = 0x00000002,  //IsDir
	RFC_DRN = 0x00000004,  //DirectoryFileReferenceNumber
	RFC_CRT = 0x00000008,  //CreationTime
	RFC_CHT = 0x00000010,  //ChangeTime
	RFC_LWT = 0x00000020,  //LastWriteTime
	RFC_LAT = 0x00000040,  //LastAccessTime
	RFC_DS = 0x00000080,  //DataSize
	RFC_NL = 0x00000100,  //NameLength
	RFC_MD5 = 0x00000200,  //MD5
	RFC_NAME = 0x00000400   //FileName
};

typedef struct FILTER_DIRECTORY {
	BOOL sub;
	ULONGLONG DircetoryReference;
}*PFILTER_DIRECTORY;

/*
* 搜索文件，范围过滤条件
*/
typedef struct FILE_FILTER {
	PCWSTR* pIncludeNames;                           //文件名包含 
	PCWSTR* pExcludeNames;                           //文件名不包含
	PFILTER_DIRECTORY* ParentDircetoryReferences;    //所属目录
	LONGLONG beginSize;                              //文件大小 >=
	LONGLONG endSize;                                //文件大小 <=
	BOOL onlyFile;                                   //仅需要文件
}*PFILE_FILTER;

typedef struct ENUM_FILERECORD_PARAM {
	RECORD_FIELD_CLASS* sortField;                   //排序规则
	FILE_FILTER Filter;
}*PENUM_FILERECORD_PARAM;

#pragma pack(push, 1)

typedef struct MFTFILERECORD {                //转存文件记录结构
											  //ULONGLONG FileReferenceNumber;         //0x00000001
	bool IsDir;                              //0x00000002
	ULONGLONG DirectoryFileReferenceNumber;
	//	ULONGLONG CreationTime;              //0x00000008 
	//	ULONGLONG ChangeTime;                //0x00000010
	//	ULONGLONG LastWriteTime;             //0x00000020
	//	ULONGLONG LastAccessTime;            //0x00000040
	ULONGLONG DataSize;                      //0x00000080
	UCHAR NameLength;                        //0x00000100
	BYTE MD5[16];                  //0x00000200
	WCHAR Name[MAX_PATH + 1];                //0x00000400
}*PMFTFILERECORD;

#pragma pack(pop)


class IVolumeInterface;
//使用者回掉接口
class IVolumeEvent {
public:
	virtual VOID  OnSearchCompeleted(IVolumeInterface*) = 0;
	virtual VOID  OnRenewDump(IVolumeInterface*) = 0;
	virtual VOID  OnFindDupCompeleted(IVolumeInterface*) = 0;
};

//用于动态库导出接口。
class IVolumeInterface {
public:
	virtual BOOL  OpenVolume(PWSTR) = 0;
	virtual PCWSTR  GetVolumeGuid() = 0;
	virtual BOOL  OpenVolumePath(const PWSTR wsz_volume) = 0;
	virtual PCWSTR  GetVolumePath() = 0;
	virtual BOOL  SetDumpFilePath(PCWSTR wsz_path) = 0;
	virtual BOOL  UnLoadDumpFile() = 0;
	virtual PCWSTR  GetDumpFileName() = 0;
	virtual VOID  SetHolper(IVolumeEvent* pHoler) = 0;

	virtual BOOL  UpdateMftDump(BOOL async) = 0;
	virtual BOOL  ListenFileChange() = 0;

	virtual BOOL  SearchFile(PFILE_FILTER pFilter, BOOL asyn) = 0;
};

class IWJLibInterface {
public:
	virtual UINT  GetVolumeCount() = 0;
	virtual IVolumeInterface*  GetVolume(ULONG idx) = 0;
	virtual BOOL  SetDumpFilePath(PCWSTR wsz_path) = 0;      //设置转存文件存放路径
};

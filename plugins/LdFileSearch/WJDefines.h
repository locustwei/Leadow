#pragma once

enum WJ_ERROR_CODE
{
	WJERROR_SUCCEED,                      //0成功 
	WJERROR_UNKNOWN,                      //1未知错误
	WJERROR_CAN_OPEN_VOLUME_HANDLE,       //2不能打开Volume句柄CreateFile失败
	WJERROR_FILE_SYSTEM_NOT_SUPPORT,      //3文件系统不支持
	WJERROR_INIT_MFT_READER,              //4初始化MftReader错误。不能读取BPB记录
	WJERROR_CAN_NOT_OPEN_INDEX_FILE,      //5不能够读写MFT索引文件
	WJERROR_NOT_EXISTS,
	WJERROR_NOT_IMPL
};

#ifdef _DEBUG
#define WJS_API 
#else WJS_CALL
#define WJS_API __declspec(dllexport)
#endif
#define WJS_CALL cdecl

interface WJS_API IWJMftFileData
{
	virtual UINT64 Size() = 0;  
	virtual PUINT64 DataClusters() = 0;
	virtual UINT ClustersCount() = 0;
	virtual ULONG Offset() = 0;     
};

interface WJS_API IWJMftFileRecord//: IInterface
{
	virtual UINT64  FileReferences() = 0;                     //数组所属目录
	virtual UINT64  ParentDircetoryReferences() = 0;          //数组所属目录
	virtual PFILETIME  CreationTime() = 0;
	virtual PFILETIME  ChangeTime() = 0;
	virtual PFILETIME  LastWriteTime() = 0;
	virtual PFILETIME  LastAccessTime() = 0;
	virtual UINT  FileAttributes() = 0;
	virtual UINT64  Size() = 0;
	//virtual USHORT  FileNameLength() = 0;
	virtual WCHAR*  FileName() = 0;
	virtual const TCHAR*  MatchExpression() = 0;             //文件过滤，该文件匹配的过滤表达式
	virtual IWJMftFileData*  GetDataStream() = 0;
	virtual BOOL  IsDir() = 0;
	//virtual TCHAR* FileType() = 0;
};

interface WJS_API IWJSHandler//: public IInterface
{
	virtual VOID  OnBegin(PVOID) = 0;
	virtual VOID  OnEnd(PVOID) = 0;
	virtual VOID  OnError(WJ_ERROR_CODE, PVOID) = 0;
	virtual BOOL  Stop(PVOID) = 0;
};


interface WJS_API IWJMftSearchHandler : public IWJSHandler
{
	/*
	* 搜索文件，范围过滤条件
	*/
	virtual const TCHAR**  NameMatchs() = 0;                           //数组,文件匹配（通配符） 
	virtual const TCHAR**  NameNotMatchs() = 0;                        //数组,文件存在目录
	//virtual const TCHAR**  ExcludeDircetory() = 0;                     //数组，文件不属于目录
	virtual INT64  MinSize() = 0;                                      //文件大小 >=
	virtual INT64  MaxSize() = 0;                                      //文件大小 <=
	virtual UCHAR  FileOrDir() = 0;                                    //仅需要文件 0:all;1:file;2:dir

	virtual VOID  OnMftFileInfo(IWJMftFileRecord*, TCHAR*, PVOID) = 0;
};

enum WJFILE_CHANGED_REASON
{
	REASON_FILE_UNKONW,
	REASON_FILE_DELETE,
	REASON_FILE_CREATE,
	REASON_RENAME_NEW_NAME,
	REASON_DATA_OVERWRITE
};

interface WJS_API IWJMftChangeHandler : public IWJSHandler
{
	virtual BOOL OnFileChange(WJFILE_CHANGED_REASON, IWJMftFileRecord*) = 0;
};

typedef enum WJFILESYSTEM_TYPE
{
	FILESYSTEM_TYPE_UNKNOWN = 0,
	FILESYSTEM_TYPE_NTFS,  // = FILESYSTEM_STATISTICS_TYPE_NTFS,     //1
	FILESYSTEM_TYPE_FAT,  // = FILESYSTEM_STATISTICS_TYPE_FAT,      //2
	FILESYSTEM_TYPE_EXFAT, //= FILESYSTEM_STATISTICS_TYPE_EXFAT,    //3
	FILESYSTEM_TYPE_REFS,  // FILESYSTEM_STATISTICS_TYPE_REFS      //4
	FILESYSTEM_TYPE_UDF
};

//用于动态库导出接口。
interface WJS_API IWJVolume
{
	virtual HANDLE OpenHandle() = 0;
	virtual VOID CloseHandle() = 0;
	virtual const TCHAR* GetVolumeGuid() = 0;
	virtual const TCHAR* GetVolumePath() = 0;
	virtual const TCHAR* GetVolumeLabel() = 0;
	virtual const TCHAR* GetShlDisplayName() = 0;
	virtual WJFILESYSTEM_TYPE  GetFileSystem() = 0;
	virtual MEDIA_TYPE  GetMediaType() = 0;
	virtual UINT64  GetTotalSize() = 0;
};

interface WJS_API IWJMftIndexFile
{
	virtual VOID  Close() = 0;

	virtual VOID StopListener() = 0;
};

interface WJS_API IWJMftReader
{
	virtual WJ_ERROR_CODE EnumMftFiles(IWJMftSearchHandler*, PVOID) = 0;
	virtual WJ_ERROR_CODE EnumDeleteFiles(IWJMftSearchHandler*, PVOID) = 0;
	virtual IWJMftFileRecord* GetFile(UINT64 FileNumber, bool OnlyName = true) = 0;
	virtual BOOL ReadSector(UINT64 sector, UINT count, PVOID buffer) = 0;
	virtual BOOL ReadCluster(UINT64 Cluster, UINT count, PVOID buffer) = 0;
	virtual USHORT GetBytesPerSector() = 0;
	virtual USHORT GetSectorsPerCluster() = 0;
	virtual UINT64 GetTotalCluster() = 0;
};

interface WJS_API IWJLibInterface
{
	virtual ~IWJLibInterface()
	{
	};
	virtual UINT  GetVolumeCount() = 0;
	virtual IWJVolume*  GetVolume(int idx) = 0;
	virtual WJ_ERROR_CODE VolumeCanReader(IWJVolume*) = 0;
	virtual IWJMftReader* CreateMftReader(IWJVolume*) = 0;
	virtual WJ_ERROR_CODE SearchVolume(IWJVolume*, IWJMftSearchHandler*) = 0;
	virtual WJ_ERROR_CODE SearchDeletedFile(IWJVolume*, IWJMftSearchHandler*) = 0;

	virtual WJ_ERROR_CODE SearchIndexFile(IWJMftIndexFile*, IWJMftSearchHandler*) = 0;
	//virtual WJ_ERROR_CODE ListenFileChaged(IWJVolume*, IWJMftIndexFile*, IWJMftChangeHandler*) = 0;
	virtual IWJMftIndexFile* CreateIndexFile(IWJVolume*, const TCHAR*, IWJSHandler*, BOOL ListenChange) = 0;
};
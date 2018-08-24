#pragma once

enum WJ_ERROR_CODE
{
	WJERROR_SUCCEED,                      //0�ɹ� 
	WJERROR_UNKNOWN,                      //1δ֪����
	WJERROR_CAN_OPEN_VOLUME_HANDLE,       //2���ܴ�Volume���CreateFileʧ��
	WJERROR_FILE_SYSTEM_NOT_SUPPORT,      //3�ļ�ϵͳ��֧��
	WJERROR_INIT_MFT_READER,              //4��ʼ��MftReader���󡣲��ܶ�ȡBPB��¼
	WJERROR_CAN_NOT_OPEN_INDEX_FILE,      //5���ܹ���дMFT�����ļ�
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
	virtual UINT64  FileReferences() = 0;                     //��������Ŀ¼
	virtual UINT64  ParentDircetoryReferences() = 0;          //��������Ŀ¼
	virtual PFILETIME  CreationTime() = 0;
	virtual PFILETIME  ChangeTime() = 0;
	virtual PFILETIME  LastWriteTime() = 0;
	virtual PFILETIME  LastAccessTime() = 0;
	virtual UINT  FileAttributes() = 0;
	virtual UINT64  Size() = 0;
	//virtual USHORT  FileNameLength() = 0;
	virtual WCHAR*  FileName() = 0;
	virtual const TCHAR*  MatchExpression() = 0;             //�ļ����ˣ����ļ�ƥ��Ĺ��˱��ʽ
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
	* �����ļ�����Χ��������
	*/
	virtual const TCHAR**  NameMatchs() = 0;                           //����,�ļ�ƥ�䣨ͨ����� 
	virtual const TCHAR**  NameNotMatchs() = 0;                        //����,�ļ�����Ŀ¼
	//virtual const TCHAR**  ExcludeDircetory() = 0;                     //���飬�ļ�������Ŀ¼
	virtual INT64  MinSize() = 0;                                      //�ļ���С >=
	virtual INT64  MaxSize() = 0;                                      //�ļ���С <=
	virtual UCHAR  FileOrDir() = 0;                                    //����Ҫ�ļ� 0:all;1:file;2:dir

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

//���ڶ�̬�⵼���ӿڡ�
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
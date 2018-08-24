#pragma once
#include "..\RecordFile.h"
#include "..\MFT\MftReader.h"

#pragma pack(push, 1)

typedef struct VOLUME_MFT_INFO {
	USN LastUsn;
	UINT64 FileCount;
	//BOOL Md5ed;
};

#pragma pack(pop)

class CWJMftIndexFile 
	: public IWJMftIndexFile
{
	friend class CEnumFilesThread;
	friend class CListenChangeThread;
	friend class CCreateFileThread;

public:
	CWJMftIndexFile(const TCHAR* Filename);
	~CWJMftIndexFile();
	BOOL Open();

	USN GetLastUSN();
	VOID SetLastUSN(USN usn);
	BOOL UpdateFile();
	VOID AddFileRecord(PMFT_FILE_DATA file);
	VOID FileRecordChange(UINT64 FileReferenceNumber, PMFT_FILE_DATA file);
	VOID DeleteFileRecord(UINT64 FileReferenceNumber);
	BOOL GetFileInfo(UINT64 FileReferenceNumber, PMFT_FILE_DATA file);
	UINT64 EnumFiles(IWJMftSearchHandler*, PVOID Param);

	UINT GetFileFullname(UINT64, CLdString&);
	void SetVolumePath(const TCHAR* volumepath);
	const TCHAR* GetVolumePath();
	IWJVolume* GetVolume();

	bool CreateIndexFile(IWJVolume* volume, IWJSHandler* hander, BOOL ListenChange);
private:
	bool m_Freed;                      //free是等待线程结束
	CRITICAL_SECTION m_CreateCriticalSection;  //free是等待 创建 线程结束,
	CRITICAL_SECTION m_ListenCriticalSection;  //free是等待 监听文件变更 线程结束,
	CRITICAL_SECTION m_EnumCriticalSection;  //free是等待 枚举文件 线程结束,

	CLdString m_FileName;
	CRecordFile m_IndexFile;
	VOLUME_MFT_INFO m_MftInfo;
	IWJMftSearchHandler * m_Handler;
	PVOID m_Param;
	CLdString m_VolumePath;
	IWJVolume* m_Volume;

	void CreateNewThreadTerminated();
	BOOL m_ListenChange;
	bool StartListenChange();
	HANDLE m_ListenChangeEvent;
protected:
	virtual VOID Close() override;
	virtual VOID StopListener() override;
};


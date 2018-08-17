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

	bool CreateIndexFile(IWJVolume* volume, IWJSHandler* hander);
private:
	CLdString m_FileName;
	CRecordFile m_IndexFile;
	VOLUME_MFT_INFO m_MftInfo;
	IWJMftSearchHandler * m_Handler;
	PVOID m_Param;
	CLdString m_VolumePath;
protected:
	virtual VOID Close() override;
};


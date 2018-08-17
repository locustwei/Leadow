#pragma once

#include "..\MFT\MftReader.h"
#include "..\WJDefines.h"
#include "WJMftFileImpl.h"
#include "..\RecordFile.h"

class CWJMftReader : public IWJMftReader
	, IMftReaderHandler
	, IMftDeleteReaderHandler

{
	friend class CEnumMftFilesThread;

public:
	~CWJMftReader();
	const TCHAR* GetVolumePath();

	static CWJMftReader* CreateReader(IWJVolume* volume);
	//static CWJMftReader* CreateReader(CWJMftIndexFile* idxfile);

protected:
	virtual BOOL EnumMftDeleteFileCallback(PMFT_FILE_DATA, PFILE_DATA_STREAM, PVOID) override;

protected:
	virtual BOOL EnumMftFileCallback(PMFT_FILE_DATA, PVOID) override;

protected:

	virtual WJ_ERROR_CODE EnumMftFiles(IWJMftSearchHandler*, PVOID) override;


	virtual WJ_ERROR_CODE EnumDeleteFiles(IWJMftSearchHandler*, PVOID) override;


	virtual IWJMftFileRecord* GetFile(UINT64 FileNumber, bool OnlyName = true) override;


	virtual BOOL ReadSector(UINT64 sector, UINT count, PVOID buffer) override;


	virtual BOOL ReadCluster(UINT64 Cluster, UINT count, PVOID buffer) override;

	virtual USHORT GetBytesPerSector() override;


	virtual USHORT GetSectorsPerCluster() override;


	virtual UINT64 GetTotalCluster() override;
private:
	CWJMftReader();
	CMftReader* m_Reader;
	CRecordFile* m_FolderCachFile;
	CLdString m_PathName;      //
	CWJMftFileImpl m_File;
	IWJMftSearchHandler* m_SearchHandler;
	CLdString m_VolumePath;

	UINT GetPathName(UINT64 FileReferenceNumber, bool read = true);
	BOOL FilterFile(PMFT_FILE_DATA pFileInfo);
	BOOL FilterDelFile(PMFT_FILE_DATA pFileInfo);
};


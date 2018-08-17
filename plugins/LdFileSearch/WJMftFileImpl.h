#pragma once
#include "..\MFT\MftReader.h"
#include "..\WJDefines.h"

class CWJMftFileDataImpl: public IWJMftFileData
{
public:
	CWJMftFileDataImpl();
	~CWJMftFileDataImpl();
	virtual UINT64 Size() override;
	virtual PUINT64 DataClusters() override;
	virtual UINT ClustersCount() override;
	virtual ULONG Offset() override;	
	void SetDataInfo(PFILE_DATA_STREAM stream);
private:
	FILE_DATA_STREAM m_DataInfo;
};

class CWJMftFileImpl :
	public IWJMftFileRecord
{
public:
	CWJMftFileImpl();
	~CWJMftFileImpl();

	virtual BOOL  IsDir() override;

	virtual PFILETIME  CreationTime() override;

	virtual PFILETIME  ChangeTime() override;

	virtual PFILETIME  LastWriteTime() override;

	virtual PFILETIME  LastAccessTime() override;

	virtual UINT64  Size() override;

	virtual WCHAR*  FileName() override;

	virtual IWJMftFileData*  GetDataStream() override;

	virtual UINT64  FileReferences() override;

	virtual UINT64  ParentDircetoryReferences() override;

	//virtual USHORT  FileNameLength() override;

	virtual PMFT_FILE_DATA  GetData(PDWORD len);

	VOID SetFileInfo(PMFT_FILE_DATA pFile, DWORD Length);


	virtual UINT FileAttributes() override;


	virtual const TCHAR* MatchExpression() override;
	void SetMatchExp(const TCHAR*);

	void SetDataInfo(PFILE_DATA_STREAM stream);
private:
	MFT_FILE_DATA m_FileInfo;
	CWJMftFileDataImpl* m_FileData;

	const TCHAR* m_MatchExp;
};


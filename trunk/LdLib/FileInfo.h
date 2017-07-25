#pragma once
#include "ldstring.h"
#include "LdArray.h"

class CFileInfo
{
	friend class CFindFileCallbackImpl;

public:
	CFileInfo();
	CFileInfo(TCHAR* pFileName);
	~CFileInfo();
	FILETIME GetCreateTime();
	FILETIME GetLastWriteTime();
	FILETIME GetLastAccessTime();
	INT64 GetAllocateSize();
	INT64 GetDataSize();
	DWORD GetAttributes();
	const TCHAR* GetFullName() const;
	const TCHAR* GetFileName() const;
	VOID SetFileName(TCHAR* pFileName);
	bool IsDirectory();
	UINT GetFilesCount();
	//VOID SetFileData(TCHAR* pFolder, )
	static CLdString FormatFileSize(INT64 nSize);
private:
	WIN32_FILE_ATTRIBUTE_DATA m_AttributeData;
	FILE_STANDARD_INFO m_StandrardInfo;
	CLdString m_Path;
	CLdString m_FileName;
	CLdString m_Name; //
	CFileInfo* m_Folder;
	CLdArray<CFileInfo*> m_Files;

	DWORD FindFiles();
	void ClearValue();
	bool GetFileBasicInfo();
	bool GetFileStandardInfo();
	void AddFile(PWIN32_FIND_DATAW pData);
};


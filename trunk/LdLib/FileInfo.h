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

	const TCHAR* GetFullName();
	const TCHAR* GetFileName();
	bool SetFileName(TCHAR* pFileName);
	bool IsDirectory();

	UINT GetFilesCount() const;
	DWORD FindFiles();

	CFileInfo* GetFolder() const;

	FILETIME GetCreateTime();
	FILETIME GetLastWriteTime();
	FILETIME GetLastAccessTime();
	INT64 GetDataSize();
	DWORD GetAttributes();
private:
	typedef struct FILE_ATTRIBUTE_DATA {
		DWORD dwFileAttributes;
		FILETIME ftCreationTime;
		FILETIME ftLastAccessTime;
		FILETIME ftLastWriteTime;
		UINT64 nFileSize;
	} *PFILE_ATTRIBUTE_DATA;

	FILE_ATTRIBUTE_DATA m_AttributeData;
	CLdString m_Path;
	CLdString m_FileName;
	CFileInfo* m_Folder;
	CLdArray<CFileInfo*> m_Files;

	void ClearValue();
	bool GetFileBasicInfo();
	void AddFile(PWIN32_FIND_DATAW pData);
};


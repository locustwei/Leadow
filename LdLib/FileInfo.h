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

	bool GetFullName(CLdString& result);
	const TCHAR* GetFileName() const;
	const TCHAR* GetPath();
	bool SetFileName(TCHAR* pFileName);
	bool IsDirectory();

	UINT GetFilesCount() const;
	DWORD FindFiles();

	CFileInfo* GetFolder();

	FILETIME GetCreateTime();
	FILETIME GetLastWriteTime();
	FILETIME GetLastAccessTime();
	INT64 GetDataSize();
	DWORD GetAttributes();
private:
	WIN32_FILE_ATTRIBUTE_DATA m_AttributeData;
	CLdString m_Path;
	CLdString m_FileName;
	CFileInfo* m_Folder;
	CLdArray<CFileInfo*> m_Files;

	void ClearValue();
	bool GetFileBasicInfo();
	void AddFile(PWIN32_FIND_DATAW pData);
};


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

	//更改对象指向文件文件
	bool SetFileName(TCHAR* pFileName);
	bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
	//父目录
	CFileInfo* GetFolder() const;
	//枚举目录下的子文件
	//if bTree 则递归枚举子目录文件
	DWORD FindFiles(bool bTree = false);
	//目录下的文件
	CLdArray<CFileInfo*>* GetFiles();
	int AddFile(CFileInfo* pFile);
	//子文件数
	UINT GetFilesCount() const;
	//扩展数据
	UINT_PTR GetTag() const;
	void SetTag(UINT_PTR Tag);
	//
	CFileInfo* Find(TCHAR* pName, bool bSub = false);
	void Sort();
	//文件属性-----------------------------------------------------------------------------
	TCHAR* GetFullName();
	TCHAR* GetFileName();
	FILETIME GetCreateTime();
	FILETIME GetLastWriteTime();
	FILETIME GetLastAccessTime();
	INT64 GetDataSize();
	DWORD GetAttributes();
	bool IsDirectory();
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
	UINT_PTR m_Tag;

	void ClearValue();
	bool GetFileBasicInfo();
	int AddFile(PWIN32_FIND_DATA pData);
};


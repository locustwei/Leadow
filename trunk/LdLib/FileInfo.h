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

	//���Ķ���ָ���ļ��ļ�
	bool SetFileName(TCHAR* pFileName);
	bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
	//��Ŀ¼
	CFileInfo* GetFolder() const;
	//ö��Ŀ¼�µ����ļ�
	//if bTree ��ݹ�ö����Ŀ¼�ļ�
	DWORD FindFiles(bool bTree = false);
	//Ŀ¼�µ��ļ�
	CLdArray<CFileInfo*>* GetFiles();
	int AddFile(CFileInfo* pFile);
	//���ļ���
	UINT GetFilesCount() const;
	//��չ����
	UINT_PTR GetTag() const;
	void SetTag(UINT_PTR Tag);
	//
	CFileInfo* Find(TCHAR* pName, bool bSub = false);
	void Sort();
	//�ļ�����-----------------------------------------------------------------------------
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


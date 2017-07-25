#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"

class CFindFileCallbackImpl: 
	public IGernalCallback<LPWIN32_FIND_DATA>,
	public ISortCompare<CFileInfo*>
{
public:
	int ArraySortCompare(CFileInfo** it1, CFileInfo** it2) override
	{
		return _tcsicmp((*it1)->m_FileName, (*it2)->m_FileName);
	};

	BOOL GernalCallback_Callback(_WIN32_FIND_DATAW* pData, UINT_PTR Param) override
	{
		CFileInfo* pFolder = (CFileInfo*)Param;
		pFolder->AddFile(pData);
		return true;
	};
};

CFileInfo::CFileInfo()
{
	ClearValue();
}

CFileInfo::CFileInfo(TCHAR * pFileName)
{
	ClearValue();
	SetFileName(pFileName);
}

CFileInfo::~CFileInfo()
{
}

FILETIME CFileInfo::GetCreateTime()
{
	GetFileBasicInfo();
	return m_AttributeData.ftCreationTime;
}

FILETIME CFileInfo::GetLastWriteTime()
{
	GetFileBasicInfo();
	return m_AttributeData.ftLastWriteTime;
}

FILETIME CFileInfo::GetLastAccessTime()
{
	GetFileBasicInfo();
	return m_AttributeData.ftLastAccessTime;
}

INT64 CFileInfo::GetDataSize()
{
	GetFileBasicInfo();
	INT64 result = m_AttributeData.nFileSizeHigh;
	result = result << 32;
	result = result | m_AttributeData.nFileSizeLow;
	return result;
}

DWORD CFileInfo::GetAttributes()
{
	if (GetFileBasicInfo())
		return m_AttributeData.dwFileAttributes;
	else
		return DWORD(-1);
}

bool CFileInfo::GetFullName(CLdString& result)
{
	if (m_FileName.IsEmpty())
		return false;

	result = GetPath();

	result += '\\';
	result += m_FileName;
	return true;
}

const TCHAR* CFileInfo::GetFileName() const
{
	return m_FileName;
}

const TCHAR* CFileInfo::GetPath()
{
	if (m_Path.IsEmpty())
	{
		if (m_Folder)
			m_Folder->GetFullName(m_Path);
	}
	return m_Path;

}

bool CFileInfo::SetFileName(TCHAR * pFileName)
{
	ClearValue();
	if (pFileName == nullptr)
		return false;
	TCHAR fileName[260] = { 0 };
	if (CFileUtils::ExtractFilePath(pFileName, fileName) == 0)
		return false;
	m_Path = fileName;
	if (CFileUtils::ExtractFileName(pFileName, fileName) == 0)
		return false;
	m_FileName = fileName;
	return true;
}

bool CFileInfo::IsDirectory()
{
	DWORD dwAtt = GetAttributes();
	return dwAtt != DWORD(-1) && (dwAtt & FILE_ATTRIBUTE_DIRECTORY);
}

UINT CFileInfo::GetFilesCount() const
{
	return m_Files.GetCount();
}

DWORD CFileInfo::FindFiles()
{
	if(!IsDirectory())
		return DWORD(-1);
	
	CLdString fullName;
	GetFullName(fullName);

	CFindFileCallbackImpl impl;

	DWORD result = CFileUtils::FindFile(fullName, _T("*.*"), &impl, (UINT_PTR)this);
	if (m_Files.GetCount() > 0)
		m_Files.Sort(&impl);
	return result;
}

CFileInfo* CFileInfo::GetFolder()
{	
	return m_Folder;
}

void CFileInfo::ClearValue()
{
	m_Folder = nullptr;
	ZeroMemory(&m_AttributeData, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	m_AttributeData.nFileSizeHigh = -1;  //标志没数据
}

bool CFileInfo::GetFileBasicInfo()
{
	if (m_AttributeData.nFileSizeHigh == -1)
	{
		CLdString fullName;
		GetFullName(fullName);
		if (GetFileAttributesEx(fullName, GetFileExInfoStandard, &m_AttributeData))
		{
			if (m_AttributeData.nFileSizeHigh == -1)
				m_AttributeData.nFileSizeHigh = 0;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void CFileInfo::AddFile(PWIN32_FIND_DATAW pData)
{
	CFileInfo* file = new CFileInfo();
	file->m_FileName = pData->cFileName;
	file->m_AttributeData.nFileSizeHigh = pData->nFileSizeHigh;
	file->m_AttributeData.nFileSizeLow = pData->nFileSizeLow;
	file->m_AttributeData.dwFileAttributes = pData->dwFileAttributes;
	file->m_AttributeData.ftCreationTime = pData->ftCreationTime;
	file->m_AttributeData.ftLastAccessTime = pData->ftLastAccessTime;
	file->m_AttributeData.ftLastWriteTime = pData->ftLastWriteTime;
	file->m_Folder = this;

	m_Files.Add(file);
}

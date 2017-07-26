#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"
#include "LdLib.h"

// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
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
	return m_AttributeData.nFileSize;
}

DWORD CFileInfo::GetAttributes()
{
	if (GetFileBasicInfo())
		return m_AttributeData.dwFileAttributes;
	else
		return DWORD(-1);
}

const TCHAR* CFileInfo::GetFullName()
{
	if (m_FileName.IsEmpty())
		return nullptr;

	return m_FileName;
}

const TCHAR* CFileInfo::GetFileName()
{
	const TCHAR* p = GetFullName();
	if (!p)
		return nullptr;
	TCHAR* s = _tcsrchr((TCHAR*)p, '\\');
	if (!s)
		return nullptr;
	return ++s;
}

bool CFileInfo::SetFileName(TCHAR * pFileName)
{
	if (pFileName == nullptr)
		return false;
	
	ClearValue();

	m_FileName = pFileName;
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
	
	CFindFileCallbackImpl impl;

	DWORD result = CFileUtils::FindFile(m_FileName, _T("*.*"), &impl, (UINT_PTR)this);
	if (m_Files.GetCount() > 0)
		m_Files.Sort(&impl);
	return result;
}

CFileInfo* CFileInfo::GetFolder() const
{	
	return m_Folder;
}

void CFileInfo::ClearValue()
{
	m_Folder = nullptr;
	ZeroMemory(&m_AttributeData, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
	m_AttributeData.nFileSize = -1;  //标志没数据
}

bool CFileInfo::GetFileBasicInfo()
{
	if (m_AttributeData.nFileSize == -1)
	{
		if (GetFileAttributesEx(m_FileName, GetFileExInfoStandard, &m_AttributeData))
		{
			if (m_AttributeData.nFileSize == -1)
				m_AttributeData.nFileSize = 0;
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
	file->m_FileName = m_FileName;
	file->m_FileName += '\\';
	file->m_FileName += pData->cFileName;
	file->m_AttributeData.nFileSize = MAKEINT64(pData->nFileSizeLow, pData->nFileSizeHigh);

	file->m_AttributeData.dwFileAttributes = pData->dwFileAttributes;
	file->m_AttributeData.ftCreationTime = pData->ftCreationTime;
	file->m_AttributeData.ftLastAccessTime = pData->ftLastAccessTime;
	file->m_AttributeData.ftLastWriteTime = pData->ftLastWriteTime;
	file->m_Folder = this;

	m_Files.Add(file);
	m_AttributeData.nFileSize += file->m_AttributeData.nFileSize;
}

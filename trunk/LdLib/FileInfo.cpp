#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"

class CFindFileCallbackImpl: 
	public IGernalCallback<LPWIN32_FIND_DATA>
{
public:
	BOOL GernalCallback_Callback(_WIN32_FIND_DATAW* pData, UINT_PTR Param) override
	{
		CFileInfo* pFolder = (CFileInfo*)Param;
		pFolder->AddFile(pData);
	};
};

CFileInfo::CFileInfo():
	m_Name()
{
	ClearValue();
}

CFileInfo::CFileInfo(TCHAR * pFileName):
	m_Name()
{
	ClearValue();
	m_Name = pFileName;
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

INT64 CFileInfo::GetAllocateSize()
{
	GetFileStandardInfo();
	return m_StandrardInfo.AllocationSize.QuadPart;
}

INT64 CFileInfo::GetDataSize()
{
	GetFileStandardInfo();
	return m_StandrardInfo.EndOfFile.QuadPart;
}

DWORD CFileInfo::GetAttributes()
{
	if (GetFileBasicInfo())
		return m_AttributeData.dwFileAttributes;
	else
		return DWORD(-1);
}

const TCHAR* CFileInfo::GetFullName() const
{
	return m_Name;
}

const TCHAR* CFileInfo::GetFileName() const
{
	if(m_Name.IsEmpty())
		return nullptr;
	else
		return _tcsrchr(m_Name.GetData(), '\\');
}

VOID CFileInfo::SetFileName(TCHAR * pFileName)
{
	ClearValue();
	m_Name = pFileName;
}

bool CFileInfo::IsDirectory()
{
	DWORD dwAtt = GetAttributes();
	return dwAtt != DWORD(-1) && (dwAtt & FILE_ATTRIBUTE_DIRECTORY);
}

CLdString CFileInfo::FormatFileSize(INT64 nSize)
{
#define __KB 1024
#define __MB (1024 * __KB)
#define __GB (1024 * __MB)
#define __TB 0x10000000000 
	double Size = nSize;
	CLdString s((UINT)100);
	if (Size < __KB)
		s.Format(_T("%.2fBytes"), Size);
	else if (Size < __MB)
		s.Format(_T("%.2fKB"), Size / __KB);
	else if (Size < __GB)
		s.Format(_T("%.2fMB"), Size / __MB);
	else if (Size < __TB)
		s.Format(_T("%.2fGB"), Size / __GB);
	else
		s.Format(_T("%.2fTB"), Size / __TB);
	return s;
}

DWORD CFileInfo::FindFiles()
{
	if (m_Name.IsEmpty())
		return DWORD(-1);
	if(!IsDirectory())
		return DWORD(-1);
	
	CFindFileCallbackImpl impl;

	return CFileUtils::FindFile(m_Name, _T("*.*"), &impl, (UINT_PTR)this);
}

void CFileInfo::ClearValue()
{
	m_Folder = nullptr;
	ZeroMemory(&m_AttributeData, sizeof(FILE_BASIC_INFO));
	m_AttributeData.nFileSizeHigh = -1;  //标志没数据
	ZeroMemory(&m_StandrardInfo, sizeof(FILE_STANDARD_INFO));
	m_StandrardInfo.EndOfFile.QuadPart = -1;
	m_Name.Empty();
}

bool CFileInfo::GetFileBasicInfo()
{
	if (m_AttributeData.nFileSizeHigh == -1)
	{
		if (GetFileAttributesEx(m_Name, GetFileExInfoStandard, &m_AttributeData))
		{
			if (m_AttributeData.nFileSizeHigh == -1)
				m_AttributeData.nFileSizeHigh = 0;
		}
		else
			return false;
	}
	return true;
}

bool CFileInfo::GetFileStandardInfo()
{
	bool result = true;
	if (m_StandrardInfo.EndOfFile.QuadPart == -1)
	{
		if (!m_Name.IsEmpty())
		{
			HANDLE hFile = CreateFile(m_Name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				result = GetFileInformationByHandleEx(hFile, FileStandardInfo, &m_StandrardInfo, sizeof(FILE_STANDARD_INFO));
				CloseHandle(hFile);
			}
			else
				result = false;
		}
	}

	return result;
}

void CFileInfo::AddFile(PWIN32_FIND_DATAW pData)
{
	CFileInfo* file = new CFileInfo();
	file->m_Name = pData->cFileName;
	file->m_AttributeData.nFileSizeHigh = pData->nFileSizeHigh;
	file->m_AttributeData.nFileSizeLow = pData->nFileSizeLow;
	file->m_AttributeData.dwFileAttributes = pData->dwFileAttributes;
	file->m_AttributeData.ftCreationTime = pData->ftCreationTime;
	file->m_AttributeData.ftLastAccessTime = pData->ftLastAccessTime;
	file->m_AttributeData.ftLastWriteTime = pData->ftLastWriteTime;
	file->m_Folder = this;

	m_Files.Add(file);
}

#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"
#include "LdLib.h"

// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
class CFindFileCallbackImpl: 
	public IGernalCallback<LPWIN32_FIND_DATA>,
	public ISortCompare<CFileInfo*>,
	public IFindCompare<CFileInfo*>
{
public:
	bool bTree;

	int ArraySortCompare(CFileInfo** it1, CFileInfo** it2) override
	{
		return _tcsicmp((*it1)->GetFileName(), (*it2)->GetFileName());
	};

	BOOL GernalCallback_Callback(_WIN32_FIND_DATAW* pData, UINT_PTR Param) override
	{
		CFileInfo* pFolder = (CFileInfo*)Param;
		int k = pFolder->AddFile(pData);
		if(bTree && (pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CFileInfo* pFile = pFolder->m_Files[k];
			pFile->FindFiles(bTree);
		}
		return true;
	};
	int ArrayFindCompare(PVOID key, CFileInfo** it) override
	{
		return _tcsicmp((TCHAR*)key, (*it)->GetFileName());
	};
};

CFileInfo::CFileInfo() :
	m_FileName(),
	m_Path(),
	m_Files(),
	m_Folder(nullptr),
	m_Tag(0)
{
	ClearValue();
}

CFileInfo::CFileInfo(TCHAR * pFileName):
	m_FileName(),
	m_Path(),
	m_Files(),
	m_Folder(nullptr),
	m_Tag(0)
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

void CFileInfo::SetTag(UINT_PTR Tag)
{
	m_Tag = Tag;
}

CFileInfo* CFileInfo::Find(TCHAR* pName, bool bSub)
{
	CFindFileCallbackImpl impl;
	CFileInfo** file = m_Files.Find(pName, &impl);
	if (file)
		return *file;
	if(bSub)
	{
		for(int i=0; i<m_Files.GetCount();i++)
		{
			if(m_Files[i]->IsDirectory())
			{
				CFileInfo* f = m_Files[i]->Find(pName, bSub);
				if (f)
					return f;
			}
		}
	}
	return nullptr;
}

void CFileInfo::Sort()
{
	CFindFileCallbackImpl impl;
	m_Files.Sort(&impl);
}

TCHAR* CFileInfo::GetFullName()
{
	if (m_FileName.IsEmpty())
		return nullptr;

	return m_FileName;
}

TCHAR* CFileInfo::GetFileName()
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

bool CFileInfo::SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData)
{
	ClearValue();

	m_FileName = pPath;
	if(m_FileName[m_FileName.GetLength()-1] != '\\')
		m_FileName += '\\';
	m_FileName += pData->cFileName;
	m_AttributeData.nFileSize = MAKEINT64(pData->nFileSizeLow, pData->nFileSizeHigh);

	m_AttributeData.dwFileAttributes = pData->dwFileAttributes;
	m_AttributeData.ftCreationTime = pData->ftCreationTime;
	m_AttributeData.ftLastAccessTime = pData->ftLastAccessTime;
	m_AttributeData.ftLastWriteTime = pData->ftLastWriteTime;

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

UINT_PTR CFileInfo::GetTag() const
{
	return m_Tag;
}

DWORD CFileInfo::FindFiles(bool bTree)
{
	if(!IsDirectory())
		return DWORD(-1);
	
	CFindFileCallbackImpl impl;
	impl.bTree = bTree;
	DWORD result = CFileUtils::FindFile(m_FileName, _T("*.*"), &impl, (UINT_PTR)this);
	if (m_Files.GetCount() > 0)
		m_Files.Sort(&impl);
	return result;
}

CLdArray<CFileInfo*>* CFileInfo::GetFiles()
{
	return &m_Files;
}

int CFileInfo::AddFile(CFileInfo* pFile)
{
	pFile->m_Folder = this;
	GetFileBasicInfo();
	m_AttributeData.nFileSize += pFile->GetDataSize();
	return m_Files.Add(pFile);
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
	for (int i = 0; i < m_Files.GetCount(); i++)
		delete m_Files[i];
	m_Files.Clear();
	m_Path.Empty();
	m_FileName.Empty();
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

int CFileInfo::AddFile(PWIN32_FIND_DATA pData)
{
	CFileInfo* file = new CFileInfo();
	file->SetFindData(GetFullName(), pData);
	return AddFile(file);
}

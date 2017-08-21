#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"
#include "../LdLib.h"

namespace LeadowLib {
	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	class CFindFileCallbackImpl :
		public IGernalCallback<LPWIN32_FIND_DATA>,
		public ISortCompare<CVirtualFile*>,
		public IFindCompare<CVirtualFile*>
	{
	public:
		bool bTree;

		int ArraySortCompare(CVirtualFile** it1, CVirtualFile** it2) override
		{
			return _tcsicmp((*it1)->GetFileName(), (*it2)->GetFileName());
		};

		BOOL GernalCallback_Callback(_WIN32_FIND_DATAW* pData, UINT_PTR Param) override
		{
			CFolderInfo* pFolder = (CFolderInfo*)Param;
			int k = pFolder->AddFile(pData);
			if (bTree && (pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				CFolderInfo* pFile = (CFolderInfo*)pFolder->m_Files[k];
				pFile->FindFiles(bTree);
				pFolder->m_AttributeData.nFileSize += pFile->GetDataSize();
			}

			return true;
		};
		int ArrayFindCompare(PVOID key, CVirtualFile** it) override
		{
			return _tcsicmp((TCHAR*)key, (*it)->GetFileName());
		};
	};

#pragma region CFile

	CFileInfo::CFileInfo() :
		m_FileName(),
		m_Folder(nullptr)
	{
		ClearValue();
		m_Tag = 0;
	}

	CFileInfo::~CFileInfo()
	{
		if (m_Folder)
			m_Folder->GetFiles()->Remove(this);
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

	/*
	VF_FILE_TYPE CFileInfo::GetFileType()
	{
		DWORD dwAtt = GetAttributes();
		if (dwAtt == DWORD(-1))
			return vft_error;
		else if (dwAtt & FILE_ATTRIBUTE_DIRECTORY)
			return vft_folder;
		else
			return vft_file;
	}
	*/


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
		if (m_FileName[m_FileName.GetLength() - 1] != '\\')
			m_FileName += '\\';
		m_FileName += pData->cFileName;
		m_AttributeData.nFileSize = MAKEINT64(pData->nFileSizeLow, pData->nFileSizeHigh);

		m_AttributeData.dwFileAttributes = pData->dwFileAttributes;
		m_AttributeData.ftCreationTime = pData->ftCreationTime;
		m_AttributeData.ftLastAccessTime = pData->ftLastAccessTime;
		m_AttributeData.ftLastWriteTime = pData->ftLastWriteTime;

		return true;
	}

	CVirtualFile* CFileInfo::GetFolder()
	{
		return m_Folder;
	}

	void CFileInfo::ClearValue()
	{
		m_Folder;
		ZeroMemory(&m_AttributeData, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
		m_AttributeData.nFileSize = -1;  //标志没数据
		m_FileName.Empty();
	}

	bool CFileInfo::GetFileBasicInfo()
	{
		if (m_AttributeData.nFileSize == -1)
		{
			if (GetFileAttributesEx(m_FileName, GetFileExInfoStandard, &m_AttributeData))
			{
				if (m_AttributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					m_AttributeData.nFileSize = 0;
			}
			else
			{
				return false;
			}
		}
		return true;
	}
#pragma endregion

#pragma region CFolder

	int CFolderInfo::AddFile(PWIN32_FIND_DATA pData)
	{
		CFileInfo* file;
		if (pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			file = new CFolderInfo();
		else
			file = new CFileInfo();
		file->SetFindData(GetFullName(), pData);
		return AddFile(file);
	}

	CFileInfo* CFolderInfo::Find(TCHAR* pName, bool bSub)
	{
		CFindFileCallbackImpl impl;
		CVirtualFile** file = m_Files.Find(pName, &impl);
		if (file)
			return (CFileInfo*)(*file);
		if (bSub)
		{
			for (int i = 0; i < m_Files.GetCount(); i++)
			{
				if (m_Files[i]->GetFileType() == vft_folder)
				{
					CFileInfo* f = ((CFolderInfo*)m_Files[i])->Find(pName, bSub);
					if (f)
						return f;
				}
			}
		}
		return nullptr;
	}

	void CFolderInfo::Sort()
	{
		CFindFileCallbackImpl impl;
		m_Files.Sort(&impl);
	}

	UINT CFolderInfo::GetFilesCount() const
	{
		return m_Files.GetCount();
	}

	CFolderInfo::CFolderInfo() :
		m_Files()
	{

	}

	DWORD CFolderInfo::FindFiles(bool bTree)
	{
		if (GetFileType() != vft_folder)
			return DWORD(-1);

		CFindFileCallbackImpl impl;
		impl.bTree = bTree;
		DWORD result = CFileUtils::FindFile(m_FileName, _T("*.*"), &impl, (UINT_PTR)this);
		if (m_Files.GetCount() > 0)
			m_Files.Sort(&impl);
		return result;
	}

	CLdArray<CVirtualFile*>* CFolderInfo::GetFiles()
	{
		return &m_Files;
	}

	int CFolderInfo::AddFile(CFileInfo* pFile)
	{
		pFile->m_Folder = this;
		GetFileBasicInfo();
		m_AttributeData.nFileSize += pFile->GetDataSize();
		return m_Files.Add(pFile);
	}

#pragma endregion
}
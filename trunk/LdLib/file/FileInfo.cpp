#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"
#include "../LdLib.h"

namespace LeadowLib {
	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	//回掉函数实现
	class CFindFileCallbackImpl :
		public IGernalCallback<LPWIN32_FIND_DATA>,
		public ISortCompare<CVirtualFile*>,
		public IFindCompare<CVirtualFile*>
	{
	public:
		bool bTree;
		//排序
		int ArraySortCompare(CVirtualFile** it1, CVirtualFile** it2) override
		{
			return _tcsicmp((*it1)->GetFileName(), (*it2)->GetFileName());
		};
		//findfirst 枚举文件夹中文件
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
		//搜索
		int ArrayFindCompare(PVOID key, CVirtualFile** it) override
		{
			if(bTree)
				return _tcsicmp((TCHAR*)key, (*it)->GetFullName());
			else
				return _tcsicmp((TCHAR*)key, (*it)->GetFileName());  //文件查找时只比较文件名，不比较路径。
		};
	};

#pragma region CFile

	TCHAR* CVirtualFile::GetFullName()
	{
		if (m_FileName.IsEmpty())
			return nullptr;

		return m_FileName;
	}

	TCHAR* CVirtualFile::GetFileName()
	{
		const TCHAR* p = GetFullName();
		if (!p)
			return nullptr;
		TCHAR* s = _tcsrchr((TCHAR*)p, '\\');
		if (!s)
			return nullptr;
		return ++s;
	}

	TCHAR* CADStream::GetFileName()
	{
		const TCHAR* p = GetFullName();
		if (!p)
			return nullptr;
		TCHAR* s = _tcsrchr((TCHAR*)p, ':');
		if (!s)
			return nullptr;
		return ++s;
	}

	CFileInfo::CFileInfo():m_Streams(nullptr)
	{
		ClearValue();
	}

	CFileInfo::~CFileInfo()
	{
		if (m_Folder)
			m_Folder->GetFiles()->Remove(this);
		if(m_Streams)
		{
			for (int i = 0; i < m_Streams->GetCount(); i++)
				delete m_Streams->Get(i);
			delete m_Streams;
		}
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

	CLdArray<CVirtualFile*>* CFileInfo::GetFiles()
	{
		if (m_FileName.IsEmpty())
			return nullptr;

		if(!m_Streams)
		{
			CLdArray<PFILE_ADS_INFO> stream_array;
			if(CFileUtils::GetFileADSNames(GetFullName(), &stream_array) == 0)
			{
				m_Streams = new CLdArray<CVirtualFile*>();
				for (int i = 0; i < stream_array.GetCount(); i++)
				{
					CADStream* stream = new CADStream();
					stream->m_FileName = GetFullName();
					stream->m_FileName += stream_array[i]->StreamName;
					stream->m_DataSize = stream_array[i]->StreamSize.QuadPart;
					stream->m_Folder = this;
					m_Streams->Add(stream);
					delete stream_array[i];
				}
			}
		}
		return m_Streams;
	}

	void CFileInfo::ClearValue()
	{
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

	CFileInfo* CFolderInfo::Find(TCHAR* pName, bool bSub, bool bPath)
	{
		CFindFileCallbackImpl impl;
		impl.bTree = bPath;
		CVirtualFile** file = m_Files.Find(pName, &impl);
		if (file)
			return (CFileInfo*)(*file);
		if (bSub)
		{
			for (int i = 0; i < m_Files.GetCount(); i++)
			{
				if (m_Files[i]->GetFileType() == vft_folder)
				{
					CFileInfo* f = ((CFolderInfo*)m_Files[i])->Find(pName, bSub, bPath);
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
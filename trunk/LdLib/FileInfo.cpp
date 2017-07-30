#include "stdafx.h"
#include "FileInfo.h"
#include "FileUtils.h"
#include "LdLib.h"

// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
class CFindFileCallbackImpl: 
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
		if(bTree && (pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CFileInfo* pFile = (CFileInfo*)pFolder->m_Files[k];
			((CFolderInfo*)pFile)->FindFiles(bTree);
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

CVirtualFile* CFileInfo::GetFolder()
{	
	return m_Folder;
}

void CFileInfo::ClearValue()
{
	m_Folder = nullptr;
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
		for (int i = 0; i<m_Files.GetCount(); i++)
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

CFolderInfo::CFolderInfo():
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

#pragma region CVolume

CVolumeInfo::CVolumeInfo(void) :
	m_VolumeGuid(),
	m_FileSystemName(),
	m_MftData(nullptr),
	m_FileSystemFlags(FS_UNKNOW)
{
	//	m_hVolume = INVALID_HANDLE_VALUE;
}

CVolumeInfo::~CVolumeInfo(void)
{
	if (m_MftData)
		delete m_MftData;
}

DWORD CVolumeInfo::OpenVolume(TCHAR* pGuid)
{
	WCHAR Names[MAX_PATH + 1] = { 0 };
	ULONG cb = sizeof(Names);
	if (GetVolumePathNamesForVolumeName(pGuid, Names, cb, &cb))
	{
		if (wcslen(Names)>0)
			Names[wcslen(Names) - 1] = '\0';
		m_FileName = Names;
	}

	pGuid[wcslen(pGuid) - 1] = '\0';

	m_VolumeGuid = pGuid;

	return 0;
}

bool CVolumeInfo::SetFileName(TCHAR* pPath)
{
	CLdString path = (TCHAR*)pPath;
	if (pPath[wcslen(pPath) - 1] != '\\')
		path += '\\';

	CFileInfo::SetFileName(path);

	WCHAR guid[MAX_PATH + 1] = { 0 };
	ULONG cb = sizeof(guid);
	if (GetVolumeNameForVolumeMountPoint(path, guid, cb))
	{
		if (wcslen(guid) > 0)
			guid[wcslen(guid) - 1] = '\0';
		m_VolumeGuid = guid;
	}

	path.Delete(path.GetLength() - 1, 1);

	return 0;
}

HANDLE CVolumeInfo::OpenVolumeHandle() const
{
	CLdString path;
	if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid;
	else if (!m_FileName.IsEmpty())
	{
		path = _T("\\\\.\\");
		path += m_FileName;
		if (path[path.GetLength() - 1] == '\\')
			path.SetAt(path.GetLength() - 1, '\0');
	}

	return CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
}

DWORD CVolumeInfo::GetBytesPerFileRecord(PDWORD pOut)
{
	DWORD result = GetVolumeMftData();
	if (result == 0)
		*pOut = m_MftData->BytesPerFileRecordSegment;
	return result;
}

DWORD CVolumeInfo::GetMftValidSize(PUINT64 pOut)
{
	DWORD result = GetVolumeMftData();
	if (result == 0)
		*pOut = m_MftData->MftValidDataLength.QuadPart;
	return result;
}

DWORD CVolumeInfo::GetVolumeInfo()
{
	CLdString path;
	if (!m_FileName.IsEmpty())
		path = m_FileName;
	else if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid + _T("\\");
	else
		return DWORD(-1);
	CLdString VolumeNameBuffer((UINT)MAX_PATH), FileSystemNameBuffer((UINT)MAX_PATH);

	DWORD MaxFilenameLength;
	if (!GetVolumeInformation(path, VolumeNameBuffer, MAX_PATH, NULL, &MaxFilenameLength, &m_FileSystemFlags, FileSystemNameBuffer, MAX_PATH))
		return GetLastError();
	else
	{
		if (FileSystemNameBuffer == _T("NTFS"))
			m_FileSystemName = FS_NTFS;
		else if (FileSystemNameBuffer == _T("FAT32"))
			m_FileSystemName = FS_FAT32;
		else if (FileSystemNameBuffer == _T("FAT"))
		{
			UINT64 totlCluters = 0;
			GetTotalClusters(&totlCluters);
			if (totlCluters <= 0x0FF)
				m_FileSystemName = FS_FAT12;
			else
				m_FileSystemName = FS_FAT16;
		}
	}
	return 0;
}

DWORD CVolumeInfo::GetVolumeMftData()
{
	if (m_MftData)
		return 0;

	NTFS_VOLUME_DATA_BUFFER data = { 0 };
	DWORD result = 0;
	if (m_FileSystemName == FS_UNKNOW)
	{
		result = GetVolumeInfo();
	}
	if (result != 0)
		return result;
	switch (m_FileSystemName)
	{
	case FS_UNKNOW: break;
	case FS_NTFS:
		result = CNtfsUtils::GetNtfsVolumeData(this, &data);
		if(result == 0)
		{
			m_MftData = new VOLUME_MFT_DATA;
			m_MftData->BytesPerCluster = data.BytesPerCluster;
			m_MftData->BytesPerFileRecordSegment = data.BytesPerFileRecordSegment;
			m_MftData->BytesPerSector = data.BytesPerSector;
			m_MftData->ClustersPerFileRecordSegment = data.ClustersPerFileRecordSegment;
			m_MftData->FreeClusters = data.FreeClusters;
			m_MftData->MftStartLcn = data.MftStartLcn;
			m_MftData->MftValidDataLength = data.MftValidDataLength;
			m_MftData->NumberSectors = data.NumberSectors;
			m_MftData->TotalClusters = data.TotalClusters;
		}
		break;
	case FS_FAT32: break;
	case FS_FAT16: break;
	case FS_FAT12: break;
	case FS_EXFAT: break;
	default: break;
	}
	return result;
}

TCHAR* CVolumeInfo::GetVolumeGuid()
{
	return m_VolumeGuid;
}

DWORD CVolumeInfo::GetAvailableFreeSpace(PUINT64 pOut)
{
	CLdString path;
	if (!m_FileName.IsEmpty())
		path = m_FileName;
	else if (!m_VolumeGuid.IsEmpty())
		path = m_VolumeGuid + _T("\\");
	else
		return (DWORD)-1;

	ULARGE_INTEGER FreeBytesAvailable;
	if (GetDiskFreeSpaceEx(path, &FreeBytesAvailable, NULL, NULL))
		*pOut = FreeBytesAvailable.QuadPart;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetTotalFreeSpace(PUINT64 pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return (DWORD)-1;
	ULARGE_INTEGER TotalFreeSpace;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, NULL, &TotalFreeSpace))
		*pOut = TotalFreeSpace.QuadPart;
	else
		return GetLastError();
	return 0;
}

DWORD CVolumeInfo::GetTotalSize(PUINT64 pOut)
{
	if (m_VolumeGuid.IsEmpty())
		return DWORD(-1);
	ULARGE_INTEGER TotalSize;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, NULL, &TotalSize, NULL))
		*pOut = TotalSize.QuadPart;
	else
	{
		return GetLastError();
		//IOCTL_DISK_GET_LENGTH_INFO
	}
	return 0;
}

DWORD CVolumeInfo::GetClusterSize(PUINT pOut)
{
	DWORD result = GetVolumeMftData();
	if (result == 0)
		*pOut = m_MftData->BytesPerCluster;
	return result;
}

DWORD CVolumeInfo::GetSectorSize(PUINT pOut)
{
	DWORD result = GetVolumeMftData();
	if (result == 0)
		*pOut = m_MftData->BytesPerSector;
	return result;
}

DWORD CVolumeInfo::GetTotalClusters(PUINT64 pOut)
{
	DWORD result = GetVolumeMftData();
	if (result == 0)
		*pOut = m_MftData->TotalClusters.QuadPart;
	return result;
}

BOOL CVolumeInfo::HasQuota()
{
	if (m_VolumeGuid.IsEmpty())
		return 0;
	ULARGE_INTEGER FreeBytesAvailable, lpTotalNumberOfFreeBytes;
	if (GetDiskFreeSpaceEx(m_VolumeGuid, &FreeBytesAvailable, NULL, &lpTotalNumberOfFreeBytes))
		return lpTotalNumberOfFreeBytes.QuadPart > FreeBytesAvailable.QuadPart;
	else
		return 0;
}

BOOL CVolumeInfo::IsMounted()
{
	return !m_FileName.IsEmpty();
}


DWORD CVolumeInfo::GetFileSystem(VOLUME_FILE_SYSTEM* pOut)
{
	DWORD Result = 0;
	if (m_FileSystemName == FS_UNKNOW)
	{
		Result = GetVolumeInfo();
	}
	if (Result == 0)
		*pOut = m_FileSystemName;
	return Result;
}

#pragma endregion

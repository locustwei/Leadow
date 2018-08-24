#include "stdafx.h"
#include "WJMftReader.h"
#include "..\MFT\NtfsUSN.h"
#include "..\MFT\NtfsMtfReader.h"
#include "..\MFT\FatMftReader.h"
#include "..\Thread.h"
#include <shlwapi.h>
#include "..\MFT\exFatReader.h"

CWJMftReader::CWJMftReader()
{
	m_Freed = false;
	m_Reader = nullptr;
	m_FolderCachFile = nullptr;
	m_SearchHandler = nullptr;
	InitializeCriticalSection(&m_CriticalSection);
}

CWJMftReader::~CWJMftReader()
{
	m_Freed = true;
	EnterCriticalSection(&m_CriticalSection);

	if (m_FolderCachFile)
		delete m_FolderCachFile;
	if (m_Reader)
		delete m_Reader;
	LeaveCriticalSection(&m_CriticalSection);
	DeleteCriticalSection(&m_CriticalSection);
}

const TCHAR* CWJMftReader::GetVolumePath()
{
	return m_VolumePath;
}

class CEnumMftFilesThread :
	public IThreadRunable
{
public:
	CEnumMftFilesThread(CWJMftReader* Owner, int Op)
	{
		m_Owner = Owner;
		m_Op = Op;
	};
	~CEnumMftFilesThread()
	{
	};

public:
	virtual VOID ThreadBody(CThread* Sender, UINT_PTR Param) override
	{
		EnterCriticalSection(&m_Owner->m_CriticalSection);
		
		m_Owner->m_SearchHandler->OnBegin(nullptr);
		
		switch (m_Op)
		{
		case 0:
			m_Owner->m_Reader->EnumFiles(m_Owner, (PVOID)Param);
			break;
		case 1:
			m_Owner->m_Reader->EnumDeleteFiles(m_Owner, (PVOID)Param);
		default:
			break;
		}

		m_Owner->m_SearchHandler->OnEnd(nullptr);
	};

	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override
	{
	};
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override
	{
		LeaveCriticalSection(&m_Owner->m_CriticalSection);
		delete this;
	};
private:
	int m_Op;
	CWJMftReader* m_Owner;
};

WJ_ERROR_CODE CWJMftReader::EnumMftFiles(IWJMftSearchHandler* handler, PVOID Param)
{
	WJ_ERROR_CODE error = WJERROR_SUCCEED;
	if (m_FolderCachFile == nullptr)
	{
		m_FolderCachFile = new CRecordFile();
		if (!m_FolderCachFile->OpenFile(nullptr))
			error = WJERROR_CAN_NOT_OPEN_INDEX_FILE;
	}
	if (error != WJERROR_SUCCEED)
		return error;
	m_SearchHandler = handler;
	CEnumMftFilesThread* threadbody = new CEnumMftFilesThread(this, 0);
	CThread* thread = new CThread(threadbody);
	thread->Start((UINT_PTR)Param);
	return WJERROR_SUCCEED;
}

WJ_ERROR_CODE CWJMftReader::EnumDeleteFiles(IWJMftSearchHandler* handler, PVOID Param)
{
	WJ_ERROR_CODE error = WJERROR_SUCCEED;
	if (m_FolderCachFile == nullptr)
	{
		m_FolderCachFile = new CRecordFile();
		if (!m_FolderCachFile->OpenFile(nullptr))
			error = WJERROR_CAN_NOT_OPEN_INDEX_FILE;
	}
	if (error != WJERROR_SUCCEED)
		return error;
	m_SearchHandler = handler;
	CEnumMftFilesThread* threadbody = new CEnumMftFilesThread(this, 1);
	CThread* thread = new CThread(threadbody);
	thread->Start((UINT_PTR)Param);
	return WJERROR_SUCCEED;
}

IWJMftFileRecord* CWJMftReader::GetFile(UINT64 FileNumber, bool OnlyName /*= true*/)
{
	MFT_FILE_DATA file;
	if (m_Reader && m_Reader->GetFileInfo(FileNumber, &file))
	{
		m_File.SetFileInfo(&file, sizeof(MFT_FILE_DATA));
		return &m_File;
	}
	else
		return nullptr;
}

BOOL CWJMftReader::ReadSector(UINT64 sector, UINT count, PVOID buffer)
{
	return m_Reader->ReadSector(sector, count, buffer);
}

BOOL CWJMftReader::ReadCluster(UINT64 Cluster, UINT count, PVOID buffer)
{
	return m_Reader->ReadCluster(Cluster, count, buffer);
}

CWJMftReader* CWJMftReader::CreateReader(IWJVolume* volume)
{
	if (volume->OpenHandle() == INVALID_HANDLE_VALUE)
	{
		return nullptr;
	}

	CMftReader* Reader = CMftReader::CreateReader(volume->OpenHandle(), volume->GetFileSystem());
	if (Reader == nullptr)
		return nullptr;

	CWJMftReader* result = new CWJMftReader();
	result->m_Reader = Reader;
	result->m_VolumePath = volume->GetVolumePath();
	if (result->m_VolumePath.IsEmpty())
		result->m_VolumePath = volume->GetVolumeGuid();
	return result;
}

//CWJMftReader * CWJMftReader::CreateReader(CWJMftIndexFile * idxfile)
//{
//	CWJMftReader* result = new CWJMftReader();
//	result->m_FolderCachFile = idxfile->GetRecordFile();
//	result->m_VolumePath = volume->GetVolumePath();
//	return result;;
//}

USHORT CWJMftReader::GetBytesPerSector()
{
	if (!m_Reader)
		return 0;
	else
		return m_Reader->GetBytesPerSector();
}

USHORT CWJMftReader::GetSectorsPerCluster()
{
	if (!m_Reader)
		return 0;
	else
		return m_Reader->GetSectorsPerCluster();
}

UINT64 CWJMftReader::GetTotalCluster()
{
	if (!m_Reader)
		return 0;
	else
		return m_Reader->GetTotalCluster();
}

BOOL CWJMftReader::EnumMftDeleteFileCallback(PMFT_FILE_DATA pFileInfo, PFILE_DATA_STREAM stream, PVOID Param)
{
	if (m_Freed)
		return FALSE;

	if ((pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		m_FolderCachFile->WriteRecord(pFileInfo->FileReferenceNumber, (PUCHAR)pFileInfo, sizeof(MFT_FILE_DATA) - sizeof(pFileInfo->Name) + (pFileInfo->NameLength + 1) * sizeof(WCHAR));
	else
	{
		if (FilterDelFile(pFileInfo))
		{
			m_File.SetFileInfo(pFileInfo, sizeof(MFT_FILE_DATA));
			m_File.SetDataInfo(stream);
			m_SearchHandler->OnMftFileInfo(&m_File, m_PathName, Param);
		}
	}

	return !m_SearchHandler->Stop(Param);
}

BOOL CWJMftReader::EnumMftFileCallback(PMFT_FILE_DATA pFileInfo, PVOID Param)
{
	if (m_Freed)
		return FALSE;

	if (FilterFile(pFileInfo))
	{
		m_File.SetFileInfo(pFileInfo, sizeof(MFT_FILE_DATA));
		m_SearchHandler->OnMftFileInfo(&m_File, m_PathName, Param);
	}

	if ((pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		m_FolderCachFile->WriteRecord(pFileInfo->FileReferenceNumber, (PUCHAR)pFileInfo, sizeof(MFT_FILE_DATA) - sizeof(pFileInfo->Name) + (pFileInfo->NameLength + 1) * sizeof(WCHAR));
	return !m_SearchHandler->Stop(Param);
}

UINT CWJMftReader::GetPathName(UINT64 FileReferenceNumber, bool read)
{
	MFT_FILE_DATA file = { 0 };
	m_PathName.Empty();

	while (FileReferenceNumber != 5 && FileReferenceNumber != 0)
	{
		if (FileReferenceNumber < 15)      //排除$Extend文件夹下的所以文件。
		{
			m_PathName.Empty();
			return 0;
		}

		if (!m_FolderCachFile->ReadRecord(FileReferenceNumber, &file, sizeof(MFT_FILE_DATA)))
		{
			if (read && m_Reader && m_Reader->GetFileInfo(FileReferenceNumber, &file))
			{
				m_FolderCachFile->WriteRecord(FileReferenceNumber, (PUCHAR)&file, sizeof(MFT_FILE_DATA) - sizeof(file.Name) + (file.NameLength + 1) * sizeof(WCHAR));
			}
			else
			{
				m_PathName.Empty();
				return 0;
			}
		}

		if ((file.FileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			m_PathName.Empty();
			return 0;
		}

		FileReferenceNumber = file.DirectoryFileReferenceNumber;
		if (file.NameLength)
		{
			file.Name[file.NameLength] = '\\';
			file.Name[file.NameLength + 1] = '\0';
			m_PathName.Insert(0, file.Name);
		}
	}
	m_PathName.Insert(0, (TCHAR*)m_VolumePath);

	return m_PathName.GetLength();
}

BOOL CWJMftReader::FilterFile(PMFT_FILE_DATA pFileInfo)
{
	UCHAR FileOrDir = m_SearchHandler->FileOrDir();
	if (FileOrDir != 0 &&
		((FileOrDir == 1 && (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||  //要文件但这是目录
		(FileOrDir == 2 && !(pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY))))  //要目录但这是文件
		return FALSE;

	if (m_SearchHandler->MinSize() != -1 && m_SearchHandler->MinSize() > pFileInfo->DataSize)
		return FALSE;
	if (m_SearchHandler->MaxSize() != -1 && m_SearchHandler->MaxSize() < pFileInfo->DataSize)
		return FALSE;


	m_PathName.Empty();
	CLdString fullname;// = m_PathName;
	fullname += pFileInfo->Name;
	//int k = 0;

	const TCHAR** p = m_SearchHandler->NameMatchs();
	if (p)
	{
		while (*p)
		{
			if (wcschr(*p, '\\') && !m_PathName.IsEmpty())  //需要路径匹配
			{
				if (!GetPathName(pFileInfo->DirectoryFileReferenceNumber))       //这个动作尽量拖后，因为很费时间
					return FALSE;
				fullname = m_PathName;
				fullname += pFileInfo->Name;
			}
			if (PathMatchSpec(fullname, *p) || _tcsicmp(pFileInfo->Name, *p) == 0)
			{
				m_File.SetMatchExp(*p);
				break;
			}
			p++;
			//k++;
		}
		if (!*p)
			return FALSE;
	}

	if (m_PathName.IsEmpty())
	{
		if (!GetPathName(pFileInfo->DirectoryFileReferenceNumber))
			return FALSE;
	}
	fullname = m_PathName;
	fullname += pFileInfo->Name;

	p = m_SearchHandler->NameNotMatchs();
	if (p)
	{
		while (*p)
		{
			if (PathMatchSpec(fullname, *p) || _tcsicmp(fullname, *p) == 0)
			{
				break;
			}
			p++;
		}
		if (*p)
			return FALSE;
	}

	return TRUE;
}

BOOL CWJMftReader::FilterDelFile(PMFT_FILE_DATA pFileInfo)
{
	if (m_SearchHandler->MinSize() != -1 && m_SearchHandler->MinSize() > pFileInfo->DataSize)
		return FALSE;
	if (m_SearchHandler->MaxSize() != -1 && m_SearchHandler->MaxSize() < pFileInfo->DataSize)
		return FALSE;


	m_PathName.Empty();
	CLdString fullname;// = m_PathName;
	fullname += pFileInfo->Name;
	//int k = 0;

	const TCHAR** p = m_SearchHandler->NameMatchs();
	if (p)
	{
		while (*p)
		{
			if (wcschr(*p, '\\') && !m_PathName.IsEmpty())  //需要路径匹配
			{
				if (!GetPathName(pFileInfo->DirectoryFileReferenceNumber)) 				
					m_PathName.Format(_T("%sFolder%lld\\"), m_VolumePath, pFileInfo->DirectoryFileReferenceNumber);
				fullname = m_PathName;
				fullname += pFileInfo->Name;
			}
			if (PathMatchSpec(fullname, *p) || _tcsicmp(pFileInfo->Name, *p) == 0)
			{
				m_File.SetMatchExp(*p);
				break;
			}
			p++;
			//k++;
		}
		if (!*p)
			return FALSE;
	}

	if (m_PathName.IsEmpty())
	{
		if (!GetPathName(pFileInfo->DirectoryFileReferenceNumber))
			m_PathName.Format(_T("%sFolder%lld\\"), m_VolumePath, pFileInfo->DirectoryFileReferenceNumber);
		fullname = m_PathName;
		fullname += pFileInfo->Name;

	}

	p = m_SearchHandler->NameNotMatchs();
	if (p)
	{
		while (*p)
		{
			if (PathMatchSpec(fullname, *p) || _tcsicmp(fullname, *p) == 0)
			{
				break;
			}
			p++;
		}
		if (*p)
			return FALSE;
	}

	return TRUE;
}

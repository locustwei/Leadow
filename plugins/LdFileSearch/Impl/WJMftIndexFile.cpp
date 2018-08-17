#include "stdafx.h"
#include "WJMftIndexFile.h"
#include "..\MFT\MftReader.h"
#include "WJVolume.h"
#include "WJMftFileImpl.h"
#include <shlwapi.h>

CWJMftIndexFile::CWJMftIndexFile(const TCHAR* Filename)
	: m_IndexFile()
	, m_Handler(nullptr)
	, m_Param(nullptr)
{
	m_FileName = Filename;
	ZeroMemory(&m_MftInfo, sizeof(m_MftInfo));
	//Open();
}


CWJMftIndexFile::~CWJMftIndexFile()
{
	UpdateFile();
}

VOID WJS_CALL CWJMftIndexFile::Close()
{
	m_IndexFile.WriteOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
	delete this;
}

UINT CWJMftIndexFile::GetFileFullname(UINT64 FileReferenceNumber, CLdString& OutName)
{
	MFT_FILE_DATA file = { 0 };
	OutName.Empty();

	while (FileReferenceNumber != 5 && FileReferenceNumber != 0)
	{
		if (!m_IndexFile.ReadRecord(FileReferenceNumber, &file, sizeof(MFT_FILE_DATA)))
		{
			return 0;
		}

		FileReferenceNumber = file.DirectoryFileReferenceNumber;
		file.Name[file.NameLength] = '\\';
		file.Name[file.NameLength + 1] = '\0';

		OutName.Insert(0, file.Name);
	}
	OutName.Insert(0, (TCHAR*)m_VolumePath);

	return OutName.GetLength();
}

void CWJMftIndexFile::SetVolumePath(const TCHAR* volumepath)
{
	m_VolumePath = volumepath;
}

const TCHAR * CWJMftIndexFile::GetVolumePath()
{
	return m_VolumePath;
}

class CCreateFileThread :
	public IThreadRunable,
	IMftReaderHandler
{
public:
	CCreateFileThread(CWJMftIndexFile* Owner, CMftReader* reader, IWJSHandler* hander)
	{
		m_Owner = Owner;
		m_Reader = reader;
		m_Hander = hander;
	};
	~CCreateFileThread()
	{
	};


	virtual BOOL EnumMftFileCallback(PMFT_FILE_DATA pFile, PVOID Param) override
	{
		m_Owner->AddFileRecord(pFile);
		if (m_Hander)
			return !m_Hander->Stop(Param);
		else
			return TRUE;
	}

public:
	virtual VOID ThreadBody(CThread* Sender, UINT_PTR Param) override
	{
		if(m_Hander)
			m_Hander->OnBegin(nullptr);

		m_Reader->EnumFiles(this, (PVOID)Param);
		if(m_Hander)
			m_Hander->OnEnd(nullptr);
	};

	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override
	{

	};
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override
	{
		delete m_Reader;
		delete this;
	};
private:
	CWJMftIndexFile* m_Owner;
	CMftReader* m_Reader;
	IWJSHandler* m_Hander;
};

bool CWJMftIndexFile::CreateIndexFile(IWJVolume* volume, IWJSHandler* hander)
{
	if (volume == nullptr || volume->OpenHandle() == INVALID_HANDLE_VALUE)
		return false;
	CMftReader* read = CMftReader::CreateReader(volume->OpenHandle(), volume->GetFileSystem());
	if (read == nullptr)
		return false;

	if (!Open())
	{
		delete read;
		return false;
	}
	m_IndexFile.DeleteAllRecord();
	
	CCreateFileThread* threadbody = new CCreateFileThread(this, read, hander);
	CThread* thread = new CThread(threadbody);
	thread->Start(0);

	m_MftInfo.LastUsn = CWJVolume::GetLastUsn(volume);
	m_VolumePath = volume->GetVolumePath();
	return true;
}

USN CWJMftIndexFile::GetLastUSN()
{
	return m_MftInfo.LastUsn;
}

VOID CWJMftIndexFile::SetLastUSN(USN usn)
{
	m_MftInfo.LastUsn = usn;
}

BOOL CWJMftIndexFile::UpdateFile()
{
	m_IndexFile.WriteOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
	return m_IndexFile.SaveFile();
}

VOID CWJMftIndexFile::AddFileRecord(PMFT_FILE_DATA file)
{
	if (m_IndexFile.WriteRecord(file->FileReferenceNumber, (PUCHAR)file, sizeof(MFT_FILE_DATA) - sizeof(file->Name) + (file->NameLength + 1) * sizeof(WCHAR)))
		m_MftInfo.FileCount++;
}

VOID CWJMftIndexFile::FileRecordChange(UINT64 FileReferenceNumber, PMFT_FILE_DATA file)
{
	m_IndexFile.WriteRecord(FileReferenceNumber, (PUCHAR)file, sizeof(MFT_FILE_DATA) - sizeof(file->Name) + (file->NameLength + 1) * sizeof(WCHAR));
}

VOID CWJMftIndexFile::DeleteFileRecord(UINT64 FileReferenceNumber)
{
	m_IndexFile.DeleteRecord(FileReferenceNumber);
	m_MftInfo.FileCount--;
}

BOOL CWJMftIndexFile::GetFileInfo(UINT64 FileReferenceNumber, PMFT_FILE_DATA file)
{
	return m_IndexFile.ReadRecord(FileReferenceNumber, file, sizeof(MFT_FILE_DATA)) > 0;
}

class CEnumFilesThread :
	public IThreadRunable
	, IRecordFileHandler
{
public:
	CEnumFilesThread(CWJMftIndexFile* Owner, IWJMftSearchHandler* hander)
	{
		m_Owner = Owner;
		m_SearchHandler = hander;
	};
	~CEnumFilesThread()
	{
	};
protected:
	virtual LONG EnumRecordCallback(UINT64, const PUCHAR data, USHORT len, PVOID Param) override
	{
		if (FilterFile((PMFT_FILE_DATA)data))
		{
			m_File.SetFileInfo((PMFT_FILE_DATA)data, len);
			m_SearchHandler->OnMftFileInfo(&m_File, m_PathName, Param);
		}
		return !m_SearchHandler->Stop(Param);
	}


	virtual LONG RecordComparer(UINT64, const PUCHAR, USHORT, UINT64, const PUCHAR, USHORT, PVOID Param) override
	{
		return 0;
	}

public:
	virtual VOID ThreadBody(CThread* Sender, UINT_PTR Param) override
	{
		if (m_SearchHandler)
			m_SearchHandler->OnBegin(nullptr);

		m_Owner->m_IndexFile.EnumRecord(this, (PVOID)Param);

		if (m_SearchHandler)
			m_SearchHandler->OnEnd(nullptr);
	};

	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override
	{

	};
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override
	{
		delete this;
	};
private:
	CWJMftIndexFile* m_Owner;
	IWJMftSearchHandler* m_SearchHandler;
	CWJMftFileImpl m_File;
	CLdString m_PathName;

	BOOL GetPathName(UINT64 FileReferenceNumber)
	{
		return m_Owner->GetFileFullname(FileReferenceNumber, m_PathName);
	}

	BOOL FilterFile(PMFT_FILE_DATA pFileInfo)
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
};

UINT64 CWJMftIndexFile::EnumFiles(IWJMftSearchHandler * hander, PVOID Param)
{
	CEnumFilesThread* threadbody = new CEnumFilesThread(this, hander);
	CThread* thread = new CThread(threadbody);
	thread->Start((UINT_PTR)Param);

	return m_MftInfo.FileCount;
}

BOOL CWJMftIndexFile::Open()
{
	if (m_IndexFile.OpenFile(m_FileName))
	{
		m_IndexFile.ReadOptionRecord(0, (PUCHAR)&m_MftInfo, sizeof(m_MftInfo));
		return TRUE;
	}
	else
		return FALSE;
}

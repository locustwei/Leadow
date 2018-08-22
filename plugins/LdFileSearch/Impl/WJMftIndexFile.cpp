#include "stdafx.h"
#include "WJMftIndexFile.h"
#include "..\MFT\MftReader.h"
#include "WJVolume.h"
#include "WJMftFileImpl.h"
#include <shlwapi.h>
#include "..\MFT\MftChangeListener.h"

CWJMftIndexFile::CWJMftIndexFile(const TCHAR* Filename)
	: m_IndexFile()
	, m_Handler(nullptr)
	, m_Param(nullptr)
{
	m_FileName = Filename;
	ZeroMemory(&m_MftInfo, sizeof(m_MftInfo));
	m_ListenChangeEvent = INVALID_HANDLE_VALUE;
	//Open();
}


CWJMftIndexFile::~CWJMftIndexFile()
{
	if (m_ListenChangeEvent != INVALID_HANDLE_VALUE)
	{
		SetEvent(m_ListenChangeEvent);
		CloseHandle(m_ListenChangeEvent);
	}

	UpdateFile();
}

void CWJMftIndexFile::CreateNewThreadTerminated()
{
	m_MftInfo.LastUsn = CWJVolume::GetLastUsn(m_Volume->OpenHandle());
	UpdateFile();

	if (m_ListenChange)
	{
		StartListenChange();
	}
}


class CListenChangeThread :
	public IThreadRunable,
	IMftChangeListenerHandler
{
public:
	CListenChangeThread(CWJMftIndexFile* Owner, HANDLE stopEvent, CMftChangeListener* ListenChange)
	{
		m_Owner = Owner;
		m_StopEvet = stopEvent;
		m_ListenChange = ListenChange;
	};
	~CListenChangeThread()
	{
	};


	virtual BOOL OnFileChangedCallback(USN_CHANGED_REASON reason, PMFT_FILE_DATA pFile, UINT_PTR Param) override
	{
		MFT_FILE_DATA tmp;

		switch (reason)
		{
		case USN_FILE_DELETE:
			m_Owner->DeleteFileRecord(pFile->FileReferenceNumber);
			break;
		case USN_FILE_CREATE:
			GetFileData(pFile);

			m_Owner->AddFileRecord(pFile);
			break;
		case USN_RENAME_NEW_NAME:
			if (m_Owner->GetFileInfo(pFile->FileReferenceNumber, &tmp))
			{
				tmp.DirectoryFileReferenceNumber = pFile->DirectoryFileReferenceNumber;
				tmp.NameLength = pFile->NameLength;
				CopyMemory(tmp.Name, pFile->Name, pFile->NameLength * sizeof(WCHAR));
				tmp.Name[tmp.NameLength] = '\0';
				m_Owner->AddFileRecord(pFile);
			}
			else
				m_Owner->AddFileRecord(pFile);


			break;
		//case USN_FILE_UNKONW:
		//case USN_DATA_OVERWRITE:
		default:
			return TRUE;
		}

		return m_Owner->m_ListenChange;
	}


	virtual BOOL Stop(UINT_PTR) override
	{
		if (m_Owner->m_MftInfo.LastUsn != m_ListenChange->GetLastUsn())
		{
			m_Owner->m_MftInfo.LastUsn = m_ListenChange->GetLastUsn();
			m_Owner->UpdateFile();
		}
		return !m_Owner->m_ListenChange;
	}

public:
	virtual VOID ThreadBody(CThread* Sender, UINT_PTR Param) override
	{
		m_ListenChange->ListenUpdate(m_StopEvet, this, Param);
	};

	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override
	{

	};
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override
	{
		if (m_ListenChange)
			delete m_ListenChange;
		delete this;
	};
private:
	CWJMftIndexFile* m_Owner;
	HANDLE m_StopEvet;
	CMftChangeListener* m_ListenChange;

	void GetFileData(PMFT_FILE_DATA pFile)
	{
		CLdString tmppath;
		m_Owner->GetFileFullname(pFile->DirectoryFileReferenceNumber, tmppath);
		if (tmppath.IsEmpty())
			return;
		tmppath += pFile->Name;

		WIN32_FIND_DATA data = { 0 };

		HANDLE hFind = FindFirstFile(tmppath, &data);
		if (hFind == INVALID_HANDLE_VALUE)
			return;
		pFile->FileAttributes = data.dwFileAttributes;
		pFile->DataSize = (data.nFileSizeHigh << 32) + data.nFileSizeLow;
		LocalFileTimeToFileTime(&data.ftCreationTime, &pFile->CreationTime);
		LocalFileTimeToFileTime(&data.ftLastWriteTime, &pFile->LastWriteTime);
		FindClose(hFind);
	}


};

bool CWJMftIndexFile::StartListenChange()
{
	CMftChangeListener* changer = CMftChangeListener::CreateListener((TCHAR*)m_Volume->GetVolumePath(), m_Volume->GetFileSystem());
	if (!changer)
		return false;
	changer->SetLastUsn(m_MftInfo.LastUsn);
	m_ListenChangeEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (m_ListenChangeEvent == INVALID_HANDLE_VALUE)
	{
		delete changer;
		return false;
	}

	CThread* thread = new CThread(new CListenChangeThread(this, m_ListenChangeEvent, changer));
	thread->Start(0);
	return true;
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
		m_Owner->CreateNewThreadTerminated();

		delete m_Reader;
		delete this;
	};
private:
	CWJMftIndexFile* m_Owner;
	CMftReader* m_Reader;
	IWJSHandler* m_Hander;
};

bool CWJMftIndexFile::CreateIndexFile(IWJVolume* volume, IWJSHandler* hander, BOOL ListenChange)
{
	if (volume == nullptr || volume->OpenHandle() == INVALID_HANDLE_VALUE || volume->GetFileSystem() != FILESYSTEM_TYPE_NTFS)
		return false;
	if (!Open())
		return false;
	
	m_Volume = volume;
	m_ListenChange = ListenChange;
	m_VolumePath = m_Volume->GetVolumePath();

	if (m_MftInfo.FileCount == 0)
	{
		CMftReader* reader = CMftReader::CreateReader(volume->OpenHandle(), volume->GetFileSystem());
		if (reader == nullptr)
			return false;
		CCreateFileThread* threadbody = new CCreateFileThread(this, reader, hander);
		CThread* thread = new CThread(threadbody);
		thread->Start(0);
	}
	else
	{
		hander->OnBegin(0);
		StartListenChange();
		hander->OnEnd(0);
	}

	return true;
}

VOID CWJMftIndexFile::StopListener()
{
	if (m_ListenChangeEvent != INVALID_HANDLE_VALUE)
		SetEvent(m_ListenChangeEvent);
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

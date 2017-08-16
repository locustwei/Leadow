#include "stdafx.h"
#include "VolumeEx.h"
#include "../../MftLib/MftReader.h"

#define TEST_temp_path _T("___Leadow_Test_tmp\\")

CVolumeEx::CVolumeEx():
	m_FileCount(0),
	m_FileTrackCount(0),
	m_Writespeed(0),
	m_Cratespeed(0),
	m_Deletespeed(0),
	m_DirectoryCount(0),
	m_RecoverableCount(0),
	m_TempPath()
{
}


CVolumeEx::~CVolumeEx()
{
}

UINT64 CVolumeEx::GetFileCount()
{
	return m_FileCount;
}

UINT64 CVolumeEx::GetTrackCount()
{
	return m_FileTrackCount;
}

UINT64 CVolumeEx::GetRemoveableCount()
{
	return m_RecoverableCount;
}

UINT CVolumeEx::GetWriteSpeed()
{
	return m_Writespeed;
}

UINT CVolumeEx::GetCrateSpeed()
{
	return m_Cratespeed;
}

UINT CVolumeEx::GetDelSpeed()
{
	return m_Deletespeed;
}

DWORD CVolumeEx::StatisticsFileStatus()
{
	DWORD result;
	result = CountFiles();
	if (result == 0)
	{
		m_TempPath = GetFullName();
		m_TempPath += TEST_temp_path;
		result = CFileUtils::ForceDirectories(m_TempPath);
	}
	if (result == 0)
	{
		m_Writespeed = TestWriteSpeed();
		TestCreateAndDelSpeed();
	}
	RemoveDirectory(m_TempPath);
	return result;
}

UINT64 CVolumeEx::GetDirectoryCount()
{
	return m_DirectoryCount;
}

DWORD CVolumeEx::CreateTempFile(CLdString& FileName)
{
	CLdString tmpName;

	CFileUtils::GenerateRandomFileName(10, &tmpName);

	tmpName.Insert(0, m_TempPath);
	HANDLE hFile = CreateFile(tmpName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return GetLastError();

	CloseHandle(hFile);
	FileName = tmpName;

	return 0;
}

UINT CVolumeEx::TestWriteSpeed()
{
	UINT result = 0;
	CLdString fileName = m_TempPath + _T("speedtest.data");

	DWORD dwTick = GetTickCount();

	FILE* f = _wfopen(fileName, _T("w+"));
	if (f)
	{
		//m_Tmpfiles.Add(fileName);
		PUCHAR Buffer = new UCHAR[1024 * 1024];
		for (int i = 0; i<10; i++)
		{
			fwrite(Buffer, sizeof(UCHAR), 1024 * 1024, f);
		}
		fseek(f, 0, 0);
		fclose(f);
		DeleteFile(fileName);
		result = GetTickCount() - dwTick;
		delete Buffer;
	}

	return result;
}

UINT CVolumeEx::TestCreateAndDelSpeed()
{
	CLdArray<CLdString> TmpFiles;

	DWORD dwTick = GetTickCount();
	for (int i = 0; i<100; i++)
	{
		CLdString FileName;
		if (CreateTempFile(FileName) != 0)
			break;
		TmpFiles.Add(FileName);
	}
	m_Cratespeed = GetTickCount() - dwTick;

	dwTick = GetTickCount();
	for(int i=0; i<TmpFiles.GetCount(); i++)
	{
		DeleteFile(TmpFiles[i]);
	}
	m_Deletespeed = GetTickCount() - dwTick;

	return 0;
}

DWORD CVolumeEx::CountFiles()
{
	DWORD result = 0;
	CMftReader* reader = CMftReader::CreateReader(this);
	if (!reader)
		return GetLastError();
	reader->SetHolder(this);
	if (reader->EnumFiles(0) == 0)
		result = GetLastError();
	delete reader;
	return result;
}

BOOL CVolumeEx::EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param)
{
	if (pFileInfo)
	{
		if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DELETED)
		{
			m_FileTrackCount++;
			if (pFileInfo->DataSize > 0)
				m_RecoverableCount++;
		}else if(pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			m_DirectoryCount++;
		}
		else
			m_FileCount++;
	}
	return true;
}

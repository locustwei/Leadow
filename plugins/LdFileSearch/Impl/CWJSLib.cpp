#include "stdafx.h"
#include "CWJSLib.h"
#include "..\MFT\MftReader.h"
#include "..\MFT\NtfsMtfReader.h"
#include "..\MFT\FatMftReader.h"
#include <shlwapi.h>
#include "WJMftFileImpl.h"
#include "WJMftReader.h"
#include "WJMftIndexFile.h"
#include <tchar.h>


CWJSLib::CWJSLib():
	m_Volumes()
{
	EnumDiskVolumes();
}

CWJSLib::~CWJSLib()
{
	for (int i = 0; i < m_Volumes.GetCount(); i++)
	{
		if(m_Volumes[i])
			delete m_Volumes[i];
	}
	m_Volumes.Clear();

	for (int i = 0; i < m_VolumeReaders.GetCount(); i++)
	{
		if (m_VolumeReaders[i])
			delete m_VolumeReaders[i];
	}
	m_VolumeReaders.Clear();
}

WJ_ERROR_CODE CWJSLib::VolumeCanReader(IWJVolume* volume)
{
	if (volume == nullptr)
		return WJERROR_UNKNOWN;
	if (volume->OpenHandle() == INVALID_HANDLE_VALUE)
		return WJERROR_CAN_OPEN_VOLUME_HANDLE;
	
	IWJMftReader* Reader = CreateMftReader(volume);
	if(Reader==nullptr)
		return WJERROR_FILE_SYSTEM_NOT_SUPPORT;
	return WJERROR_SUCCEED;
}

VOID CWJSLib::EnumDiskVolumes()
{
	TCHAR Volume[MAX_PATH] = {0};
	ULONG CharCount = MAX_PATH;

	HANDLE hFind = FindFirstVolume(Volume, ARRAYSIZE(Volume));
	if(hFind == INVALID_HANDLE_VALUE)
		return ;

	while(TRUE){

		size_t Index = wcslen(Volume) - 1;
		if(Index < 0)
			Index = 0;

		if (Volume[0]!= L'\\' || Volume[1]!= L'\\' || Volume[2]!= L'?' || Volume[3]!= L'\\' || Volume[Index] != L'\\')
		{			
			continue;
		}

		CWJVolume* volume = new CWJVolume(Volume);
		m_Volumes.Add(volume);

		ZeroMemory(Volume, MAX_PATH * sizeof(TCHAR));

		if(!FindNextVolume(hFind, Volume, ARRAYSIZE(Volume)))
		{
			break;
		}
	}

	FindVolumeClose(hFind);

}

CWJMftReader * CWJSLib::FindReader(const TCHAR * volume)
{
	for(int i=0; i<m_VolumeReaders.GetCount(); i++)
		if(_tcscmp(m_VolumeReaders[i]->GetVolumePath(), volume)==0)
			return m_VolumeReaders[i];
	return nullptr;
}

IWJMftReader * CWJSLib::CreateMftReader(IWJVolume * volume)
{
	CWJMftReader* Reader = FindReader(volume->GetVolumePath());
	if (!Reader)
	{
		Reader = CWJMftReader::CreateReader(volume);
		if(Reader)
			m_VolumeReaders.Add(Reader);
	}
	return Reader;
}

UINT  CWJSLib::GetVolumeCount()
{
	return (UINT)m_Volumes.GetCount();
}

IWJVolume* CWJSLib::GetVolume(int idx)
{
	if (idx >= m_Volumes.GetCount())
		return nullptr;
	else
		return m_Volumes[idx];
}


WJ_ERROR_CODE  CWJSLib::SearchVolume(IWJVolume* volume, IWJMftSearchHandler* handler)
{
	IWJMftReader* reader = CreateMftReader(volume);
	if (reader != nullptr)
		return reader->EnumMftFiles(handler, nullptr);
	else
		return WJERROR_FILE_SYSTEM_NOT_SUPPORT;
}


WJ_ERROR_CODE  CWJSLib::SearchDeletedFile(IWJVolume* volume, IWJMftSearchHandler* handler)
{
	IWJMftReader* reader = CreateMftReader(volume);
	if (reader != nullptr)
		return reader->EnumDeleteFiles(handler, nullptr);
	else
		return WJERROR_FILE_SYSTEM_NOT_SUPPORT;
}

IWJMftIndexFile* CWJSLib::CreateIndexFile(IWJVolume* volume, const TCHAR* Filename, IWJSHandler* hander, BOOL ListenChange)
{
	CWJMftIndexFile* file = new CWJMftIndexFile(Filename);
	if(!file->CreateIndexFile(volume, hander, ListenChange))
	{
		delete file;
		return nullptr;
	}
	else
		return file;
}

WJ_ERROR_CODE CWJSLib::SearchIndexFile(IWJMftIndexFile* idxfile, IWJMftSearchHandler* hander)
{
	CWJMftIndexFile* file = static_cast<CWJMftIndexFile*>(idxfile);
	file->EnumFiles(hander, nullptr);
	return WJERROR_SUCCEED;
}

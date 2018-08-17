#pragma once

#include "..\MFT\MftReader.h"
#include "..\WJDefines.h"

class CWJVolume : public IWJVolume
{
public:
	CWJVolume(const TCHAR* volume);
	~CWJVolume();

	virtual HANDLE  OpenHandle() override;

	virtual const TCHAR*  GetVolumeGuid() override;

	virtual const TCHAR*  GetVolumePath() override;

	virtual WJFILESYSTEM_TYPE  GetFileSystem() override;

	virtual MEDIA_TYPE  GetMediaType() override;

	virtual UINT64  GetTotalSize() override;
	virtual VOID  CloseHandle() override;

	virtual const TCHAR*  GetVolumeLabel() override;


	virtual const TCHAR* GetShlDisplayName() override;

public:
	static USN GetLastUsn(HANDLE hVolume);
private:
	HANDLE m_hVolume;
	CLdString m_VolumeGuid;
	CLdString m_VolumePath;
	WJFILESYSTEM_TYPE m_FileSystem;
	DWORD m_VolumeSerialNumber;
	DWORD m_FileSystemFlags;
	CLdString m_VolumeName;
	CLdString m_DisplayName;

	VOID GetVolumeInfo();
};


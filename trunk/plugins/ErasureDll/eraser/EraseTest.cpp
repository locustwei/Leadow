#include "stdafx.h"
#include "EraseTest.h"

#define TEST_temp_path _T("___Leadow_Test_tmp\\")

DWORD CEraseTest::CountVolumeFiles(CVolumeInfo * pVolume)
{
	DWORD result = 0;
	CMftReader* reader = CMftReader::CreateReader(pVolume);
	if (!reader)
		return GetLastError();
	reader->SetHolder(this);
	if (reader->EnumFiles(pVolume->GetTag()) == 0)
		result = GetLastError();
	delete reader;
	return result;
}

BOOL CEraseTest::EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param)
{
	if (pFileInfo)
	{
		PERASE_VOLUME_INFO info = (PERASE_VOLUME_INFO)Param;

		if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DELETED)
		{
			info->FileTrackCount++;
			if (pFileInfo->DataSize > 0)
				info->RecoverableCount++;
		}
		else if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			info->DirectoryCount++;
		}
		else
			info->FileCount++;
	}
	return true;

}

CEraseTest::CEraseTest()
{
}


CEraseTest::~CEraseTest()
{
}

DWORD CEraseTest::TestVolume(TCHAR* VolumePath, IErasureCallback* callback)
{
	DWORD result;
	ERASE_VOLUME_INFO info = { 0 };
	CVolumeInfo volume;
	volume.SetFileName(VolumePath);
	volume.SetTag((UINT_PTR)&info);
	CLdString TempPath;
	result = CountVolumeFiles(&volume);

	return result;
}

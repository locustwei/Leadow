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

UINT CEraseTest::TestWriteSpeed()
{
	return 0;
}

BOOL CEraseTest::EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param)
{
	if (pFileInfo)
	{
		PTEST_VOLUME_RESULT info = (PTEST_VOLUME_RESULT)Param;

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

//TEST_VOLUME_RESULT CEraseTest::TestVolume(TCHAR* VolumePath)
//{
//	TEST_VOLUME_RESULT result;
//	CVolumeInfo volume;
//	volume.SetFileName(VolumePath);
//	volume.SetTag((UINT_PTR)&result);
//	CLdString TempPath;
//	result.ErrorCode = CountVolumeFiles(&volume);
//
//	return result;
//}

TEST_FILE_RESULT CEraseTest::TestFile(TCHAR* lpFileName, BOOL bRemoveFolder)
{
	TEST_FILE_RESULT result = { 0 };
	
	CFileInfo* pFile;
	if (CFileUtils::IsDirectoryExists(lpFileName))
	{
		pFile = new CFolderInfo();
	}
	else
		pFile = new CFileInfo();

	if (pFile->GetFileType() == vft_folder)
	{
		((CFolderInfo*)pFile)->FindFiles(true);
		result.FileCount = ((CFolderInfo*)pFile)->GetFilesCount(true);
	}
	
	CLdArray<CADStream *>* ads = pFile->GetADStreams();
	if (ads)
	{
		result.ADSCount = ads->GetCount();
		for (int i = 0; i < result.ADSCount; i++)
			result.ADSSizie += ads->Get(i)->GetDataSize();
	}
	result.TotalSize = pFile->GetDataSize();
	return result;
}

TEST_FILE_RESULT CEraseTest::TestDirectory(TCHAR* lpDirName)
{
	TEST_FILE_RESULT result = { 0 };

	return result;
}

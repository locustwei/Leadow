#include "stdafx.h"
#include "EraseTest.h"

#define TEST_temp_path _T("___Leadow_Test_tmp\\")

DWORD CEraseTest::CountVolumeFiles(CVolumeInfo * pVolume)
{
	DWORD result = 0;
	//CMftReader* reader = CMftReader::CreateReader(pVolume);
	//if (!reader)
	//	return GetLastError();
	//reader->SetHolder(this);
	//if (reader->EnumFiles(pVolume->GetTag()) == 0)
	//	result = GetLastError();
	//delete reader;
	return result;
}

UINT CEraseTest::TestWriteSpeed()
{
	return 0;
}

BOOL CEraseTest::EnumMftFileCallback(PMFT_FILE_DATA pFileInfo, PVOID Param)
{
	if (pFileInfo)
	{
		PTEST_VOLUME_RESULT info = (PTEST_VOLUME_RESULT)Param;

		//if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DELETED)
		//{
		//	info->FileTrackCount++;
		//	if (pFileInfo->DataSize > 0)
		//		info->RecoverableCount++;
		//}
		//else if (pFileInfo->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		//{
		//	info->DirectoryCount++;
		//}
		//else
		//	info->FileCount++;
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

DWORD CEraseTest::TestFile(TCHAR* lpFileName, BOOL bRemoveFolder, PTEST_FILE_RESULT tr)
{
	DWORD result = 0;
	do 
	{
		CFileInfo* pFile;
		if (CFileUtils::IsDirectoryExists(lpFileName))
		{
			pFile = new CFolderInfo();
		}
		else
			pFile = new CFileInfo();

		pFile->SetFileName(lpFileName);

		if (pFile->GetFileType() == vft_folder)
		{
			result = ((CFolderInfo*)pFile)->FindFiles(true);
			if (result != 0)
				break;
			tr->FileCount = ((CFolderInfo*)pFile)->GetFilesCount(true);
		}

		CLdArray<CADStream *>* ads = pFile->GetADStreams();
		if (ads)
		{
			tr->ADSCount = ads->GetCount();
			for (int i = 0; i < tr->ADSCount; i++)
				tr->ADSSizie += ads->Get(i)->GetDataSize();
		}
		tr->TotalSize = pFile->GetDataSize();
	} while (FALSE);
	
	return result;
}

TEST_FILE_RESULT CEraseTest::TestDirectory(TCHAR* lpDirName)
{
	TEST_FILE_RESULT result = { 0 };

	return result;
}

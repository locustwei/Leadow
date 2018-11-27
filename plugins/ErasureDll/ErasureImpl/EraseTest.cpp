#include "stdafx.h"
#include "EraseTest.h"

#define TEST_temp_path _T("___Leadow_Test_tmp\\")

DWORD CEraseTest::CountVolumeFiles(CVolumeInfo * pVolume)
{
	DWORD result = 0;
	return result;
}

UINT CEraseTest::TestWriteSpeed()
{
	return 0;
}

CEraseTest::CEraseTest()
{
	
}


CEraseTest::~CEraseTest()
{
}

DWORD CEraseTest::TestVolume(TCHAR * VolumePath, CErasureMothed * method, BOOL bSkipSpace, BOOL bSkipTrack, PTEST_VOLUME_RESULT tr)
{
	DWORD result = 0;

	CVolumeInfo volume(VolumePath);
	tr->TotalSize = volume.GetTotalSize(&result);
	if (result != 0)
		return result;
	tr->FreeSpaceSize = volume.GetAvailableFreeSpace();
	if (result != 0)
		return result;

	CMftReader* reader = CMftReader::CreateReader(volume.OpenVolumeHandle(), volume.GetFileSystem());
	if (reader)
	{
		if(!reader->GetFileStats(&tr->FileCount, &tr->DirectoryCount, &tr->FileTrackCount))
			result = GetLastError();
		delete reader;
	}

	return result;
}

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


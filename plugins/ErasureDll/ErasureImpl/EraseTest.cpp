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

TEST_VOLUME_RESULT CEraseTest::TestVolume(TCHAR * VolumePath, CErasureMothed * method, BOOL bSkipSpace, BOOL bSkipTrack)
{
	CVolumeInfo volume(VolumePath);
	CMftReader* reader = CMftReader::CreateReader(volume.OpenVolumeHandle(), volume.GetFileSystem());
	if (reader)
	{
		reader->GetFileStats(nullptr, nullptr, nullptr);
		delete reader;
	}
	return TEST_VOLUME_RESULT();
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


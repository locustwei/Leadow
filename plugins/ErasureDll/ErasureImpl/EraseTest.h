#pragma once

/*!
 * file EraseTest.h
 *
 * author asa-pc
 * date ���� 2018
 *
 * brief 
 *
 * 
 */

#include "ErasureMethod.h"
#include "..\define.h"

/*!
 * \class classname
 *
 * \brief 
 *
 * \author asa-pc
 * \date ���� 2018
 */
class CEraseTest
	:public IMftReaderHandler
	//,IThreadRunable
{
public:
	CEraseTest();
	~CEraseTest();

	// ���Է���
	TEST_VOLUME_RESULT TestVolume(
		TCHAR* VolumePath,           //���������̷�
		CErasureMothed* method,      //�����㷨
		BOOL bSkipSpace,             //�Ƿ�����δʹ�ÿռ����(���������̿��пռ�)
		BOOL bSkipTrack
	);

	// �����ļ�
	DWORD TestFile(
		TCHAR* lpFileName,          //�ļ���
		BOOL bRemoveFolder,         //�Ƿ�ɾ�����ļ���(�����ļ������ļ�����ɾ���ļ���)
		PTEST_FILE_RESULT
	);
	//����Ŀ¼
	TEST_FILE_RESULT TestDirectory(
		TCHAR* lpDirName         //�ļ���
	);
private:
	DWORD StatisticsFileStatus(); //ͳ�ƴ����ļ�����

	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
	BOOL EnumMftFileCallback(PMFT_FILE_DATA pFileInfo, PVOID Param) override;
};


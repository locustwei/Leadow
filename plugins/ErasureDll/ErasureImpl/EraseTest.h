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
	:public IMftReadeHolder
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
	TEST_FILE_RESULT TestFile(
		TCHAR* lpFileName,         //�ļ���
		BOOL bRemoveFolder         //�Ƿ�ɾ�����ļ���(�����ļ������ļ�����ɾ���ļ���)
	);
	//����Ŀ¼
	TEST_FILE_RESULT TestDirectory(
		TCHAR* lpDirName         //�ļ���
	);
private:
	DWORD StatisticsFileStatus(); //ͳ�ƴ����ļ�����

	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
};


#pragma once
#include "ErasureMethod.h"

typedef struct TEST_VOLUME_RESULT {
	UINT64 FileCount;        //�ļ���
	UINT64 DirectoryCount;   //Ŀ¼��
	UINT64 FileTrackCount;   //MFT��ɾ���ļ��ĺۼ���
	UINT64 RecoverableCount; //ɾ���Ŀ��Ա��ָ��ļ������������ּ��ļ���С��Ϊ0��
	UINT   Writespeed;       //д�ļ��ٶȣ�1G�ֽ�����ʱ�䣬��λ���룩
	UINT   Cratespeed;       //����0�ֽ��ļ��ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	UINT   Deletespeed;      //ɾ��0�ֽ��ļ����ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	DWORD ErrorCode;        //����������루�����
}*PTEST_VOLUME_RESULT;

typedef struct TEST_FILE_RESULT {
	DWORD FileCount;        //�ļ���
	DWORD ADSCount;         //������
	UINT64 TotalSize;       //�ļ���Ŀ¼���ֽ���
	UINT64 ADSSizie;        //���ֽ���
	DWORD ErrorCode;        //����������루�����
}*PTEST_FILE_RESULT;

class CEraseTest
	:public IMftReadeHolder
	,IThreadRunable
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


#pragma once

#include "Erasure.h"
#include "../../MftLib/FatMftReader.h"

typedef struct ERASE_VOLUME_INFO {
	UINT64 FileCount;        //�ļ���
	UINT64 DirectoryCount;   //Ŀ¼��
	UINT64 FileTrackCount;   //MFT��ɾ���ļ��ĺۼ���
	UINT64 RecoverableCount; //ɾ���Ŀ��Ա��ָ��ļ������������ּ��ļ���С��Ϊ0��
	UINT   Writespeed;       //д�ļ��ٶȣ�1G�ֽ�����ʱ�䣬��λ���룩
	UINT   Cratespeed;       //����0�ֽ��ļ��ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	UINT   Deletespeed;      //ɾ��0�ֽ��ļ����ٶȣ�100���ļ�����ʱ�䣬��λ���룩
}*PERASE_VOLUME_INFO;

class CEraseTest
	:public IMftReadeHolder
{
private:
	DWORD StatisticsFileStatus(); //ͳ�ƴ����ļ�����
	
	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
public:
	CEraseTest();
	~CEraseTest();

	// Qualifier: �������̿��пռ䣬������ɾ���ļ���¼
	DWORD TestVolume(
		TCHAR* VolumePath,          //���������̷�
		IErasureCallback* callback  //�������̻ص�����
	);

	// �����ļ���
	DWORD TestFile(
		TCHAR* lpFileName,         //�ļ���
		IErasureCallback* callbck  //�������̻ص�����
								   //BOOL bRemoveFolder         //�Ƿ�ɾ�����ļ���(�����ļ������ļ�����ɾ���ļ���)
	);
	//�����ļ��У��ļ��б����ǿ��ļ��У�
	DWORD TestDirectory(
		TCHAR* lpDirName,         //�ļ���
		IErasureCallback* callbck //�������̻ص�����
	);
};


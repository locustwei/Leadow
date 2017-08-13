#pragma once
#include "../../MftLib/MftReader.h"


class CVolumeEx : public CVolumeInfo, public IMftReadeHolder
{
public:
	CVolumeEx();
	~CVolumeEx();

	UINT64 GetFileCount();
	UINT64 GetDirectoryCount();

	UINT64 GetTrackCount();
	UINT64 GetRemoveableCount();

	UINT GetWriteSpeed();
	UINT GetCrateSpeed();
	UINT GetDelSpeed();
	DWORD StatisticsFileStatus(); //ͳ�ƴ����ļ�����
private:
	UINT64 m_FileCount;      //�ļ���
	UINT64 m_DirectoryCount; //Ŀ¼��

	UINT64 m_FileTrackCount;   //MFT��ɾ���ļ��ĺۼ���
	UINT64 m_RecoverableCount; //ɾ���Ŀ��Ա��ָ��ļ������������ּ��ļ���С��Ϊ0��
	UINT   m_Writespeed;     //д�ļ��ٶȣ�1G�ֽ�����ʱ�䣬��λ���룩
	UINT   m_Cratespeed;     //����0�ֽ��ļ��ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	UINT   m_Deletespeed;    //ɾ��0�ֽ��ļ����ٶȣ�100���ļ�����ʱ�䣬��λ���룩
	CLdString m_TempPath;
	DWORD CreateTempFile(CLdString& FileName);
	UINT TestWriteSpeed();
	UINT TestCreateAndDelSpeed();
	DWORD CountFiles();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
};


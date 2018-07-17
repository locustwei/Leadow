#pragma once

/*!
�ص������������жϴ����������
*/
class IStatisticsCallback
{
public:
	virtual BOOL ErasureStart() = 0;    //��ʼ����
	virtual BOOL ErasureCompleted(DWORD dwErroCode) = 0;  //�����ꡣ
	//************************************
	// Qualifier: ����
	// Parameter: UINT64 nMaxCount  �������
	// Parameter: UINT64 nCurent  �������
	//************************************
	virtual BOOL ErasureProgress(UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class CVolumeEx : public CVolumeInfo, public IMftReaderHandler
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
	BOOL EnumMftFileCallback(PMFT_FILE_DATA pFileInfo, PVOID Param) override;
};


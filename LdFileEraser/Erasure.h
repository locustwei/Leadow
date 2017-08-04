#pragma once

#include "ErasureMethod.h"

//#define ERASE_UNUSED_SPACE 0xF1
#define ERROR_CANCELED 0xC0000001

/*!
�ص������������жϴ����������
 */
class LDLIB_API IErasureCallback
{
public:
	//************************************
	// Parameter: UINT nStepCount  �ּ������
	//************************************
	virtual BOOL ErasureStart(UINT nStepCount) = 0;
	//************************************
	// Parameter: UINT nStep  �ڼ���
	// Parameter: DWORD dwErroCode  ������루0���ɹ���
	//************************************
	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) = 0;
	//************************************
	// Qualifier: ����
	// Parameter: UINT nStep  �ڼ���
	// Parameter: UINT64 nMaxCount  �������
	// Parameter: UINT64 nCurent  �������
	//************************************
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class CErasure
{
public:
	CErasure();
	~CErasure();

	//************************************
	// Qualifier: �������̿��пռ䣬������ɾ���ļ���¼
	// Parameter: TCHAR * Driver �̷���C:��
	// Parameter: CErasureMethod & method 
	// Parameter: IErasureCallback * callback
	//************************************
	DWORD UnuseSpaceErasure(CVolumeInfo* pvolume, CErasureMethod* method, IErasureCallback* callback);
	//************************************
	// Qualifier: �����ļ���ͬʱɾ����
	// Parameter: TCHAR * lpFileName
	// Parameter: CErasureMethod & m_method
	// Parameter: IErasureCallback * callbck
	//************************************
	DWORD FileErasure(TCHAR* lpFileName, CErasureMethod* method, IErasureCallback* callbck);
private:
	CVolumeInfo* m_volInfo;
	CLdString m_tmpDir;
	//IErasureCallback* m_callback;
	CErasureMethod* m_method; 
	CLdArray<CLdString> m_Tmpfiles;
	
	//�����ļ�
	DWORD EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//д�������
	DWORD WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//д����
	DWORD WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount);

	//�������Ŀ��пռ�
	DWORD EraseFreeDataSpace(IErasureCallback* callback);
	//����nFileSize���ļ�������
	DWORD CrateTempFileAndErase(UINT64 nFileSize, IErasureCallback* callback);
	DWORD CreateTempFile(UINT64 nFileSize, HANDLE* pOut, int nFileNameLength=20);

	//MFT���пռ�
	DWORD EraseFreeMftSpace(IErasureCallback* callback);
	DWORD EraseNtfsFreeSpace(IErasureCallback* callback);
	DWORD EraseFatFreeSpace(IErasureCallback* callback);
	//����ɾ���ļ��ۼ�
	DWORD EraseDelFileTrace(IErasureCallback* callback);
	DWORD EraseNtfsTrace(IErasureCallback* callback);
	DWORD EraseFatTrace(IErasureCallback* callback);

	//�����ļ���ʱ��
	static DWORD ResetFileDate(HANDLE hFile);

	//ɾ����������ʱ�ļ�
	DWORD DeleteTempFiles(IErasureCallback* callback);
};


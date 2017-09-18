#pragma once

#include "ErasureMethod.h"
#include "VolumeEx.h"

//#define ERASE_UNUSED_SPACE 0xF1
#define ERROR_CANCELED 0xC0000001

enum ERASE_STEP
{
	ERASER_DATA_FREE,
	ERASER_FILE,
	ERASER_MFT_FREE,
	ERASER_DEL_TEMPFILE,
	ERASER_DEL_TRACK
};

/*!
�ص������������жϴ����������
 */
class IErasureCallback
{
public:
	virtual BOOL ErasureStart() = 0;    //��ʼ����
	virtual BOOL ErasureCompleted(DWORD dwErroCode) = 0;  //�����ꡣ
	//************************************
	// Qualifier: ����
	// Parameter: UINT nStep  �ڼ���
	// Parameter: UINT64 nMaxCount  �������
	// Parameter: UINT64 nCurent  �������
	//************************************
	virtual BOOL ErasureProgress(ERASE_STEP nStep, UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class CErasure
{
public:
	CErasure();
	~CErasure();

	// Qualifier: �������̿��пռ䣬������ɾ���ļ���¼
	DWORD UnuseSpaceErasure(
		CVolumeEx* pvolume,          //����������
		CErasureMethod* method,      //�����㷨
		IErasureCallback* callback,  //�������̻ص�����
		BOOL bSkipSpace,             //�Ƿ�����δʹ�ÿռ����(���������̿��пռ�)
		BOOL bSkipTrack              //�Ƿ������ļ�������ļ���ɾ���ۼ�����
	);

	// �����ļ�(�ļ��У���
	DWORD FileErasure(
		TCHAR* lpFileName,         //�ļ���
		CErasureMethod* method,    //�����㷨
		IErasureCallback* callbck, //�������̻ص�����
		BOOL bRemoveFolder         //�Ƿ�ɾ�����ļ���(�����ļ������ļ�����ɾ���ļ���)
	);
private:
	CVolumeEx* m_Volume;
	CLdString m_tmpDir;               //��ʷ�ļ�Ŀ¼��
	//IErasureCallback* m_callback;
	CErasureMethod* m_method;         //��������
	CLdArray<CLdString*> m_Tmpfiles;  //�������ɵ���ʱ�ļ���(���ʱɾ��֮)
	UINT64 m_DeleteFileTraces;        //��ɾ�����ļ�����(���̲���ʱ������Ҫʱ��)
	//�����ļ�
	DWORD EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//Buffer д���ļ���ȥ
	DWORD WriteFileContent(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, PBYTE Buffer, IErasureCallback* callbck);

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
	DWORD CreateTempDirectory();
};


#pragma once

#include "ErasureMethod.h"

#define ERASE_UNUSED_SPACE 0xF1

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
	virtual VOID ErasureCompleted(UINT nStep, DWORD dwErroCode) = 0;
	//************************************
	// Qualifier: ����
	// Parameter: UINT nStep  �ڼ���
	// Parameter: UINT64 nMaxCount  �������
	// Parameter: UINT64 nCurent  �������
	//************************************
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class LDLIB_API CUnuseSpaceErasure
{
public:
	CUnuseSpaceErasure();
	~CUnuseSpaceErasure();

	DWORD Execute(CLdString& Driver, CErasureMethod& method, IErasureCallback* callback);
private:
	CVolumeInfo m_volInfo;
	CLdString m_tmpDir;
	IErasureCallback* m_callback;
	CErasureMethod* m_method;
	CLdArray<CLdString> m_Tmpfiles;

	DWORD EraseUnusedSpace();
	DWORD EraserFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	DWORD WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	DWORD WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount);
};


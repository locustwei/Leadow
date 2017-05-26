#pragma once

#include "ErasureMethod.h"

#define ERASE_UNUSED_SPACE 0xF1

class LDLIB_API IErasureCallback
{
public:
	//************************************
	// Parameter: UINT nStepCount  分几步完成
	//************************************
	virtual BOOL ErasureStart(UINT nStepCount) = 0;
	//************************************
	// Parameter: UINT nStep  第几步
	// Parameter: DWORD dwErroCode  错误代码（0：成功）
	//************************************
	virtual VOID ErasureCompleted(UINT nStep, DWORD dwErroCode) = 0;
	//************************************
	// Qualifier: 进度
	// Parameter: UINT nStep  第几步
	// Parameter: UINT64 nMaxCount  最大数量
	// Parameter: UINT64 nCurent  完成数量
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


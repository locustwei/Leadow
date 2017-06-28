#pragma once

#include "ErasureMethod.h"

//#define ERASE_UNUSED_SPACE 0xF1
#define ERROR_CANCELED 0xC0000001

/*!
回掉函数，用于中断处理、报告进度
 */
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
	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) = 0;
	//************************************
	// Qualifier: 进度
	// Parameter: UINT nStep  第几步
	// Parameter: UINT64 nMaxCount  最大数量
	// Parameter: UINT64 nCurent  完成数量
	//************************************
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class LDLIB_API CErasure
{
public:
	CErasure();
	~CErasure();

	//************************************
	// Qualifier: 擦除磁盘空闲空间，所有已删除文件记录
	// Parameter: TCHAR * Driver 盘符（C:）
	// Parameter: CErasureMethod & method 
	// Parameter: IErasureCallback * callback
	//************************************
	DWORD UnuseSpaceErasure(TCHAR* Driver, CErasureMethod& method, IErasureCallback* callback);
	//************************************
	// Qualifier: 擦除文件，同时删除。
	// Parameter: TCHAR * lpFileName
	// Parameter: CErasureMethod & m_method
	// Parameter: IErasureCallback * callbck
	//************************************
	DWORD FileErasure(TCHAR* lpFileName, CErasureMethod& method, IErasureCallback* callbck);
private:
	CVolumeInfo m_volInfo;
	CLdString m_tmpDir;
	//IErasureCallback* m_callback;
	CErasureMethod* m_method;
	CLdArray<CLdString> m_Tmpfiles;

	DWORD UnusedSpace2TempFile(IErasureCallback* callback);
	DWORD EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	DWORD WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	DWORD WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount);
	DWORD EraseMftFileRecord(IErasureCallback* callback);
	DWORD EraseNtfsDeletedFile(IErasureCallback* callback);
	DWORD CreateTempFile(UINT64 nFileSize, HANDLE* pOut);
	DWORD ResetFileDate(HANDLE hFile);
	DWORD EraseFreeSpace(UINT64 nFileSize, IErasureCallback* callback);
	DWORD DeleteTempFiles(IErasureCallback* callback);
	void GenerateRandomFileName(int length, CLdString& Out);
	DWORD EraseMftDeleteFile(IErasureCallback* callback);
};


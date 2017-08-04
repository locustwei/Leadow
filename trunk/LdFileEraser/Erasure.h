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

class CErasure
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
	DWORD UnuseSpaceErasure(CVolumeInfo* pvolume, CErasureMethod* method, IErasureCallback* callback);
	//************************************
	// Qualifier: 擦除文件，同时删除。
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
	
	//擦除文件
	DWORD EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//写随机数据
	DWORD WriteFileRandom(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//写常理
	DWORD WriteFileConst(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck, PBYTE bytes, UINT nCount);

	//数据区的空闲空间
	DWORD EraseFreeDataSpace(IErasureCallback* callback);
	//创建nFileSize的文件并擦除
	DWORD CrateTempFileAndErase(UINT64 nFileSize, IErasureCallback* callback);
	DWORD CreateTempFile(UINT64 nFileSize, HANDLE* pOut, int nFileNameLength=20);

	//MFT空闲空间
	DWORD EraseFreeMftSpace(IErasureCallback* callback);
	DWORD EraseNtfsFreeSpace(IErasureCallback* callback);
	DWORD EraseFatFreeSpace(IErasureCallback* callback);
	//擦除删除文件痕迹
	DWORD EraseDelFileTrace(IErasureCallback* callback);
	DWORD EraseNtfsTrace(IErasureCallback* callback);
	DWORD EraseFatTrace(IErasureCallback* callback);

	//设置文件的时间
	static DWORD ResetFileDate(HANDLE hFile);

	//删除产生的临时文件
	DWORD DeleteTempFiles(IErasureCallback* callback);
};


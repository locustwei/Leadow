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
回掉函数，用于中断处理、报告进度
 */
class IErasureCallback
{
public:
	virtual BOOL ErasureStart() = 0;    //开始擦除
	virtual BOOL ErasureCompleted(DWORD dwErroCode) = 0;  //擦除完。
	//************************************
	// Qualifier: 进度
	// Parameter: UINT nStep  第几步
	// Parameter: UINT64 nMaxCount  最大数量
	// Parameter: UINT64 nCurent  完成数量
	//************************************
	virtual BOOL ErasureProgress(ERASE_STEP nStep, UINT64 nMaxCount, UINT64 nCurent) = 0;
};

class CErasure
{
public:
	CErasure();
	~CErasure();

	// Qualifier: 擦除磁盘空闲空间，所有已删除文件记录
	DWORD UnuseSpaceErasure(
		CVolumeEx* pvolume,          //待擦除磁盘
		CErasureMethod* method,      //擦除算法
		IErasureCallback* callback,  //擦除过程回掉函数
		BOOL bSkipSpace,             //是否跳过未使用空间擦除(不擦除磁盘空闲空间)
		BOOL bSkipTrack              //是否跳过文件分配表文件被删除痕迹擦除
	);

	// 擦除文件(文件夹）。
	DWORD FileErasure(
		TCHAR* lpFileName,         //文件名
		CErasureMethod* method,    //擦除算法
		IErasureCallback* callbck, //擦除过程回掉函数
		BOOL bRemoveFolder         //是否删除空文件夹(擦除文件夹中文件但不删除文件夹)
	);
private:
	CVolumeEx* m_Volume;
	CLdString m_tmpDir;               //历史文件目录名
	//IErasureCallback* m_callback;
	CErasureMethod* m_method;         //擦除方法
	CLdArray<CLdString*> m_Tmpfiles;  //保存生成的临时文件名(完成时删除之)
	UINT64 m_DeleteFileTraces;        //被删除的文件个数(磁盘擦除时计算需要时间)
	//擦除文件
	DWORD EraseFile(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, IErasureCallback* callbck);
	//Buffer 写到文件中去
	DWORD WriteFileContent(HANDLE hFile, UINT64 nStartPos, UINT64 nFileSize, PBYTE Buffer, IErasureCallback* callbck);

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
	DWORD CreateTempDirectory();
};


#pragma once
#include "ErasureMethod.h"

typedef struct TEST_VOLUME_RESULT {
	UINT64 FileCount;        //文件数
	UINT64 DirectoryCount;   //目录数
	UINT64 FileTrackCount;   //MFT中删除文件的痕迹数
	UINT64 RecoverableCount; //删除文可以被恢复文件数（粗略数字及文件大小不为0）
	UINT   Writespeed;       //写文件速度（1G字节所用时间，单位毫秒）
	UINT   Cratespeed;       //创建0字节文件速度（100个文件所用时间，单位毫秒）
	UINT   Deletespeed;      //删除0字节文件的速度（100个文件所用时间，单位毫秒）
	DWORD ErrorCode;        //分析错误代码（如果）
}*PTEST_VOLUME_RESULT;

typedef struct TEST_FILE_RESULT {
	DWORD FileCount;        //文件数
	DWORD ADSCount;         //流个数
	UINT64 TotalSize;       //文件（目录）字节数
	UINT64 ADSSizie;        //流字节数
	DWORD ErrorCode;        //分析错误代码（如果）
}*PTEST_FILE_RESULT;

class CEraseTest
	:public IMftReadeHolder
	,IThreadRunable
{
public:
	CEraseTest();
	~CEraseTest();

	// 测试分区
	TEST_VOLUME_RESULT TestVolume(
		TCHAR* VolumePath,           //待擦除磁盘法
		CErasureMothed* method,      //擦除算法
		BOOL bSkipSpace,             //是否跳过未使用空间擦除(不擦除磁盘空闲空间)
		BOOL bSkipTrack
	);

	// 测试文件
	TEST_FILE_RESULT TestFile(
		TCHAR* lpFileName,         //文件名
		BOOL bRemoveFolder         //是否删除空文件夹(擦除文件夹中文件但不删除文件夹)
	);
	//测试目录
	TEST_FILE_RESULT TestDirectory(
		TCHAR* lpDirName         //文件名
	);
private:
	DWORD StatisticsFileStatus(); //统计磁盘文件数据

	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
};


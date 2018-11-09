#pragma once

/*!
 * file EraseTest.h
 *
 * author asa-pc
 * date 三月 2018
 *
 * brief 
 *
 * 
 */

#include "ErasureMethod.h"
#include "..\define.h"

/*!
 * \class classname
 *
 * \brief 
 *
 * \author asa-pc
 * \date 三月 2018
 */
class CEraseTest
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
	DWORD TestFile(
		TCHAR* lpFileName,          //文件名
		BOOL bRemoveFolder,         //是否删除空文件夹(擦除文件夹中文件但不删除文件夹)
		PTEST_FILE_RESULT
	);

private:
	DWORD StatisticsFileStatus(); //统计磁盘文件数据

	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
};


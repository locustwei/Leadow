#pragma once

#include "Erasure.h"
#include "../../MftLib/FatMftReader.h"

typedef struct ERASE_VOLUME_INFO {
	UINT64 FileCount;        //文件数
	UINT64 DirectoryCount;   //目录数
	UINT64 FileTrackCount;   //MFT中删除文件的痕迹数
	UINT64 RecoverableCount; //删除文可以被恢复文件数（粗略数字及文件大小不为0）
	UINT   Writespeed;       //写文件速度（1G字节所用时间，单位毫秒）
	UINT   Cratespeed;       //创建0字节文件速度（100个文件所用时间，单位毫秒）
	UINT   Deletespeed;      //删除0字节文件的速度（100个文件所用时间，单位毫秒）
}*PERASE_VOLUME_INFO;

class CEraseTest
	:public IMftReadeHolder
{
private:
	DWORD StatisticsFileStatus(); //统计磁盘文件数据
	
	DWORD CountVolumeFiles(CVolumeInfo * pVolume);
	UINT TestWriteSpeed();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
public:
	CEraseTest();
	~CEraseTest();

	// Qualifier: 擦除磁盘空闲空间，所有已删除文件记录
	DWORD TestVolume(
		TCHAR* VolumePath,          //待擦除磁盘法
		IErasureCallback* callback  //擦除过程回掉函数
	);

	// 擦除文件。
	DWORD TestFile(
		TCHAR* lpFileName,         //文件名
		IErasureCallback* callbck  //擦除过程回掉函数
								   //BOOL bRemoveFolder         //是否删除空文件夹(擦除文件夹中文件但不删除文件夹)
	);
	//擦除文件夹（文件夹必须是空文件夹）
	DWORD TestDirectory(
		TCHAR* lpDirName,         //文件名
		IErasureCallback* callbck //擦除过程回掉函数
	);
};


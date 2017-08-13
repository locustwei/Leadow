#pragma once
#include "../../MftLib/MftReader.h"


class CVolumeEx : public CVolumeInfo, public IMftReadeHolder
{
public:
	CVolumeEx();
	~CVolumeEx();

	UINT64 GetFileCount();
	UINT64 GetDirectoryCount();

	UINT64 GetTrackCount();
	UINT64 GetRemoveableCount();

	UINT GetWriteSpeed();
	UINT GetCrateSpeed();
	UINT GetDelSpeed();
	DWORD StatisticsFileStatus(); //统计磁盘文件数据
private:
	UINT64 m_FileCount;      //文件数
	UINT64 m_DirectoryCount; //目录数

	UINT64 m_FileTrackCount;   //MFT中删除文件的痕迹数
	UINT64 m_RecoverableCount; //删除文可以被恢复文件数（粗略数字及文件大小不为0）
	UINT   m_Writespeed;     //写文件速度（1G字节所用时间，单位毫秒）
	UINT   m_Cratespeed;     //创建0字节文件速度（100个文件所用时间，单位毫秒）
	UINT   m_Deletespeed;    //删除0字节文件的速度（100个文件所用时间，单位毫秒）
	CLdString m_TempPath;
	DWORD CreateTempFile(CLdString& FileName);
	UINT TestWriteSpeed();
	UINT TestCreateAndDelSpeed();
	DWORD CountFiles();
	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override;
};


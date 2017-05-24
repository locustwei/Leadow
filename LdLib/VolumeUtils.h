/*!
 * 磁盘分区（Volume）。
 * 1、分区基本信息（Guid、Path）。
 * 。
*/

#pragma once

#include "stdafx.h"
#include "LdString.h"

class CVolumeInfo
{
public:
	CVolumeInfo(void);
	~CVolumeInfo(void);

	DWORD  OpenVolume(TCHAR* pGuid);         //用卷GUID，打开卷句柄
	PCWSTR  GetVolumeGuid();
	DWORD  OpenVolumePath(const TCHAR* pPath); //卷路径（盘符），打开卷句柄
	PCWSTR  GetVolumePath();
	UINT64 GetAvailableFreeSpace();
	UINT64 GetTotalFreeSpace();
	UINT64 GetTotalSize();
	UINT GetClusterSize();
	UINT GetSectorSize();
	UINT GetTotalClusters();
	BOOL HasQuota();
	BOOL IsMounted();
	void MountedVolumes();
	//查询磁盘性能 IOCTL_DISK_PERFORMANCE
	//分区信息IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS
private:

	CLdString m_VolumeGuid;           //卷GUID
	CLdString m_VolumePath;           //路径
	CLdString m_FileSystemName;
	DWORD FileSystemFlags;
	DWORD MaxFilenameLength;

	HANDLE m_hVolume;                //卷句柄（读取MFT）
	
	DWORD OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile，打开hVolume
	BOOL GetVolumeInfo();
private:  //接口函数
};

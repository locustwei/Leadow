/*!
 * 磁盘分区（Volume）。
 * 1、分区基本信息（Guid、Path）。
 * 。
*/

#pragma once

#include "stdafx.h"
#include "LdString.h"

enum VOLUME_FILE_SYSTEM
{
	FS_UNKNOW,
	FS_NTFS,
	FS_FAT32,
	FS_FAT16,
	FS_FAT12,
	FS_EXFAT
};

class CVolumeInfo
{
public:
	CVolumeInfo(void);
	~CVolumeInfo(void);

	//************************************
	// Qualifier:  初始化磁盘
	// Parameter: TCHAR * pGuid  磁盘“名称”用findvolumen返回的GUID。
	//************************************
	DWORD  OpenVolume(TCHAR* pGuid);
	PCWSTR  GetVolumeGuid();

	//************************************
	// Qualifier: 初始化磁盘
	// Parameter: const TCHAR * pPath 盘符（也可能是映射路径）
	//************************************
	DWORD  OpenVolumePath(const TCHAR* pPath); //卷路径（盘符），打开卷句柄
	PCWSTR  GetVolumePath();
	//可用空闲空间
	DWORD GetAvailableFreeSpace(PUINT64 pOut);
	//磁盘空闲空间（可能大于可用空间）
	DWORD GetTotalFreeSpace(PUINT64 pOut);
	//总空间
	DWORD GetTotalSize(PUINT64 pOut);
	//Cluster 字节数
	DWORD GetClusterSize(PUINT pOut);
	//Sector 字节数
	DWORD GetSectorSize(PUINT pOut);
	//Cluter 数
	DWORD GetTotalClusters(PUINT pOut);
	//磁盘配额（为用户分配的空间）
	BOOL HasQuota();
	//是否分配盘符
	BOOL IsMounted();
	//
	void MountedVolumes();
	//磁盘MFT格式。
	DWORD GetFileSystem(VOLUME_FILE_SYSTEM* pOut);
	//查询磁盘性能 IOCTL_DISK_PERFORMANCE
	//分区信息IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle();             //CreateFile，打开hVolume

private:

	CLdString m_VolumeGuid;           //卷GUID
	CLdString m_VolumePath;           //路径
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD FileSystemFlags;
	DWORD MaxFilenameLength;

	DWORD GetVolumeInfo();
private:  //接口函数
};

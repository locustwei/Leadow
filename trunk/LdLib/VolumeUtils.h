/*!
 * ���̷�����Volume����
 * 1������������Ϣ��Guid��Path����
 * ��
*/

#pragma once

#include "stdafx.h"
#include "LdString.h"

class CVolumeInfo
{
public:
	CVolumeInfo(void);
	~CVolumeInfo(void);

	DWORD  OpenVolume(TCHAR* pGuid);         //�þ�GUID���򿪾���
	PCWSTR  GetVolumeGuid();
	DWORD  OpenVolumePath(const TCHAR* pPath); //��·�����̷������򿪾���
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
	//��ѯ�������� IOCTL_DISK_PERFORMANCE
	//������ϢIOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS
private:

	CLdString m_VolumeGuid;           //��GUID
	CLdString m_VolumePath;           //·��
	CLdString m_FileSystemName;
	DWORD FileSystemFlags;
	DWORD MaxFilenameLength;

	HANDLE m_hVolume;                //��������ȡMFT��
	
	DWORD OpenVolumeHandle(PCWSTR wsz_path);             //CreateFile����hVolume
	BOOL GetVolumeInfo();
private:  //�ӿں���
};

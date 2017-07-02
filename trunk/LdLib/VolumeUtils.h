/*!
 * ���̷�����Volume����
 * 1������������Ϣ��Guid��Path����
 * ��
*/

#pragma once

#include "stdafx.h"
#include "LdString.h"
#include "LdArray.h"
#include "LdStructs.h"

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
	// Qualifier:  ��ʼ������
	// Parameter: TCHAR * pGuid  ���̡����ơ���findvolumen���ص�GUID��
	//************************************
	DWORD  OpenVolume(TCHAR* pGuid);
	PCWSTR  GetVolumeGuid();

	//************************************
	// Qualifier: ��ʼ������
	// Parameter: const TCHAR * pPath �̷���Ҳ������ӳ��·����
	//************************************
	DWORD  OpenVolumePath(const TCHAR* pPath); //��·�����̷������򿪾���
	PCWSTR  GetVolumePath();
	//���ÿ��пռ�
	DWORD GetAvailableFreeSpace(PUINT64 pOut);
	//���̿��пռ䣨���ܴ��ڿ��ÿռ䣩
	DWORD GetTotalFreeSpace(PUINT64 pOut);
	//�ܿռ�
	DWORD GetTotalSize(PUINT64 pOut);
	//Cluster �ֽ���
	DWORD GetClusterSize(PUINT pOut);
	//Sector �ֽ���
	DWORD GetSectorSize(PUINT pOut);
	//Cluter ��
	DWORD GetTotalClusters(PUINT pOut);
	//������Ϊ�û�����Ŀռ䣩
	BOOL HasQuota();
	//�Ƿ�����̷�
	BOOL IsMounted();
	//����MFT��ʽ��
	DWORD GetFileSystem(VOLUME_FILE_SYSTEM* pOut);
	//��ѯ�������� IOCTL_DISK_PERFORMANCE
	//������ϢIOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle() const;             //CreateFile����hVolu


private:

	CLdString m_VolumeGuid;           //��GUID
	CLdString m_VolumePath;           //·��
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD m_FileSystemFlags;

	DWORD GetVolumeInfo();
private:  //�ӿں���
};

class CVolumeUtils
{
public:
	//���̷��ķ���
	static DWORD MountedVolumes(IGernalCallback<TCHAR*>* callback, UINT_PTR Param);
	//FindFirstVolume 
	static DWORD EnumVolumeNames(IGernalCallback<TCHAR*>* callback, UINT_PTR Param);
	static VOLUME_FILE_SYSTEM GetVolumeFileSystem(TCHAR* szPath);
private:

};

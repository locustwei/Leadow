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
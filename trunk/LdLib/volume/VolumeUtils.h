/*!
 * 磁盘分区（Volume）。
 * 1、分区基本信息（Guid、Path）。
 * 。
*/

#pragma once

#include "../classes/LdString.h"
#include "../classes/LdArray.h"
#include "../ldapp/LdStructs.h"

namespace LeadowLib {
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
		//有盘符的分区
		static DWORD MountedVolumes(IGernalCallback<TCHAR*>* callback, UINT_PTR Param);
		//FindFirstVolume 
		static DWORD EnumVolumeNames(IGernalCallback<TCHAR*>* callback, UINT_PTR Param);
		static VOLUME_FILE_SYSTEM GetVolumeFileSystem(TCHAR* szPath);
	private:

	};
};
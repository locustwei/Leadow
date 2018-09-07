/*!
 * ���̷�����Volume����
 * 1������������Ϣ��Guid��Path����
 * ��
*/

#pragma once

namespace LeadowDisk {

	enum VOLUME_FILE_SYSTEM
	{
		FILESYSTEM_TYPE_UNKNOWN = 0,
		FILESYSTEM_TYPE_NTFS,  // = FILESYSTEM_STATISTICS_TYPE_NTFS,     //1
		FILESYSTEM_TYPE_FAT,  // = FILESYSTEM_STATISTICS_TYPE_FAT,      //2
		FILESYSTEM_TYPE_EXFAT, //= FILESYSTEM_STATISTICS_TYPE_EXFAT,    //3
		FILESYSTEM_TYPE_REFS,  // FILESYSTEM_STATISTICS_TYPE_REFS      //4
		FILESYSTEM_TYPE_UDF
	};

	class CVolumeUtils
	{
	public:
		//���̷��ķ���
		static DWORD MountedVolumes(CLdMethodDelegate callback, UINT_PTR Param);
		//FindFirstVolume 
		static DWORD EnumVolumeNames(CLdMethodDelegate callback, UINT_PTR Param);
		static VOLUME_FILE_SYSTEM GetVolumeFileSystem(TCHAR* szPath);
	private:

	};
};
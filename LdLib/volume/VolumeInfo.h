#pragma once

#include "../file/FileInfo.h"
#include "VolumeUtils.h"

namespace LeadowLib {
	typedef struct _VOLUME_BPB_DATA {
		DWORD BytesPerSector;
		DWORD SectorsPerCluster;
		UINT64 TotalSectors;
		UCHAR MediaType;
		DWORD BytesPerClusters;
		union
		{
			struct
			{
				DWORD BytesPerFileRecordSegment;
				DWORD ClustersPerFileRecord;
				UINT64 MftValidDataLength;
				UINT64 MftStartLcn;
			};
			struct
			{
				USHORT SectorsPerRootDirectory;
				DWORD SectorsPerFAT;
				DWORD FirstDataSector;
				DWORD FirstFatSector;
				DWORD RootClus;
			};
		};
	}VOLUME_BPB_DATA, *PVOLUME_BPB_DATA;


	class CVolumeInfo : public CFolderInfo
	{
	public:
		CVolumeInfo(void);
		~CVolumeInfo(void);

		//************************************
		// Qualifier:  初始化磁盘
		// Parameter: TCHAR * pGuid  磁盘“名称”用findvolumen返回的GUID。
		//************************************
		DWORD  OpenVolume(TCHAR* pGuid);
		TCHAR*  GetVolumeGuid();
		//************************************/*
		// Qualifier: 初始化磁盘
		// Parameter: const TCHAR * pPath 盘符（也可能是映射路径）
		//************************************
		bool SetFileName(TCHAR* pFileName) override;
		//可用空闲空间
		UINT64 GetAvailableFreeSpace(PDWORD pErrorCode = nullptr);
		//磁盘空闲空间（可能大于可用空间）
		UINT64 GetTotalFreeSpace(PDWORD pErrorCode = nullptr);
		//总空间
		UINT64 GetTotalSize(PDWORD pErrorCode = nullptr);
		//磁盘配额（为用户分配的空间）
		BOOL HasQuota();
		//是否分配盘符
		BOOL IsMounted();
		//磁盘MFT格式。
		VOLUME_FILE_SYSTEM GetFileSystem(PDWORD pErrorCode = nullptr);
		//查询磁盘性能 IOCTL_DISK_PERFORMANCE
		//分区信息IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

		HANDLE OpenVolumeHandle(PDWORD pErrorCode = nullptr) const;             //CreateFile，
		VF_FILE_TYPE GetFileType() override { return vft_volume; }

		//分区引导区信息（扇区字节数等）
		PVOLUME_BPB_DATA GetBpbData(PDWORD pErrorCode = nullptr);
		PVOLUME_BPB_DATA RefreshBpbData(); //刷新一下引导区数据，特殊时候使用
	private:
		PVOLUME_BPB_DATA m_MftData;  //NTFS_VOLUME_DATA_BUFFER;
		CLdString m_VolumeGuid;           //卷GUID
		VOLUME_FILE_SYSTEM m_FileSystemName;
		DWORD m_FileSystemFlags;
		DWORD GetVolumeInfo();
	private:  //接口函数
	};
};
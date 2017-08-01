#pragma once

#include "FileInfo.h"

typedef struct _VOLUME_BPB_DATA {
	USHORT BytesPerSector;
	UCHAR SectorsPerCluster;
	DWORD TotalSectors;
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
	//Cluster 字节数
	UINT GetClusterSize(PDWORD pErrorCode = nullptr);
	//Sector 字节数
	UINT GetSectorSize(PDWORD pErrorCode = nullptr);
	//Cluter 数
	UINT64 GetTotalClusters(PDWORD pErrorCode = nullptr);
	//磁盘配额（为用户分配的空间）
	BOOL HasQuota();
	//是否分配盘符
	BOOL IsMounted();
	//磁盘MFT格式。
	VOLUME_FILE_SYSTEM GetFileSystem(PDWORD pErrorCode = nullptr);
	//查询磁盘性能 IOCTL_DISK_PERFORMANCE
	//分区信息IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle(PDWORD pErrorCode = nullptr) const;             //CreateFile，
	VF_FILE_TYPE GetFileType() override { return vft_volume; };

	UINT GetBytesPerFileRecord(PDWORD pErrorCode = nullptr);
	UINT64 GetMftValidSize(PDWORD pErrorCode = nullptr);
private:
	typedef struct VOLUME_MFT_DATA {
		LARGE_INTEGER NumberSectors;
		LARGE_INTEGER TotalClusters;
		LARGE_INTEGER FreeClusters;
		DWORD BytesPerSector;
		DWORD BytesPerCluster;
		DWORD BytesPerFileRecordSegment;
		DWORD ClustersPerFileRecordSegment;
		LARGE_INTEGER MftValidDataLength;
		LARGE_INTEGER MftStartLcn;
	}*PVOLUME_MFT_DATA;

	PVOLUME_MFT_DATA m_MftData;  //NTFS_VOLUME_DATA_BUFFER;
	CLdString m_VolumeGuid;           //卷GUID
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD m_FileSystemFlags;
	DWORD GetVolumeInfo();
	DWORD GetVolumeMftData();
private:  //接口函数
};

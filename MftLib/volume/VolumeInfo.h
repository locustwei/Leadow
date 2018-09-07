#pragma once

#include "VolumeUtils.h"

namespace LeadowDisk {

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


	class CVolumeInfo
	{
	public:
		CVolumeInfo(TCHAR*);
		~CVolumeInfo(void);

		const TCHAR* GetVolumeGuid();
		const TCHAR* GetVolumePath();
		const TCHAR* GetShlDisplayName();
		const TCHAR* GetVolumeLabel();

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

		HANDLE OpenVolumeHandle(PDWORD pErrorCode = nullptr);
		VOID CloseHandle();

		//分区引导区信息（扇区字节数等）
		PVOLUME_BPB_DATA GetBpbData(PDWORD pErrorCode = nullptr);
		PVOLUME_BPB_DATA RefreshBpbData(); //刷新一下引导区数据，特殊时候使用

	private:
		PVOLUME_BPB_DATA m_MftData;       //NTFS_VOLUME_DATA_BUFFER;
		CLdString m_VolumeGuid;           //卷GUID
		DWORD m_FileSystemFlags;

		HANDLE m_hVolume;
		CLdString m_VolumePath;
		VOLUME_FILE_SYSTEM m_FileSystem;
		DWORD m_VolumeSerialNumber;
		CLdString m_VolumeLabel;
		CLdString m_DisplayName;

		DWORD GetVolumeInfo();

	};
};
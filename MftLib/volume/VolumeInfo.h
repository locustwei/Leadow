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

		//���ÿ��пռ�
		UINT64 GetAvailableFreeSpace(PDWORD pErrorCode = nullptr);
		//���̿��пռ䣨���ܴ��ڿ��ÿռ䣩
		UINT64 GetTotalFreeSpace(PDWORD pErrorCode = nullptr);
		//�ܿռ�
		UINT64 GetTotalSize(PDWORD pErrorCode = nullptr);
		//������Ϊ�û�����Ŀռ䣩
		BOOL HasQuota();
		//�Ƿ�����̷�
		BOOL IsMounted();
		//����MFT��ʽ��
		VOLUME_FILE_SYSTEM GetFileSystem(PDWORD pErrorCode = nullptr);

		HANDLE OpenVolumeHandle(PDWORD pErrorCode = nullptr);
		VOID CloseHandle();

		//������������Ϣ�������ֽ����ȣ�
		PVOLUME_BPB_DATA GetBpbData(PDWORD pErrorCode = nullptr);
		PVOLUME_BPB_DATA RefreshBpbData(); //ˢ��һ�����������ݣ�����ʱ��ʹ��

	private:
		PVOLUME_BPB_DATA m_MftData;       //NTFS_VOLUME_DATA_BUFFER;
		CLdString m_VolumeGuid;           //��GUID
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
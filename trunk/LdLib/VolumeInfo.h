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
	// Qualifier:  ��ʼ������
	// Parameter: TCHAR * pGuid  ���̡����ơ���findvolumen���ص�GUID��
	//************************************
	DWORD  OpenVolume(TCHAR* pGuid);
	TCHAR*  GetVolumeGuid();
	//************************************/*
	// Qualifier: ��ʼ������
	// Parameter: const TCHAR * pPath �̷���Ҳ������ӳ��·����
	//************************************
	bool SetFileName(TCHAR* pFileName) override; 
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
	//��ѯ�������� IOCTL_DISK_PERFORMANCE
	//������ϢIOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle(PDWORD pErrorCode = nullptr) const;             //CreateFile��
	VF_FILE_TYPE GetFileType() override { return vft_volume; }

	VOID RefreshBpbData();;

	PVOLUME_BPB_DATA GetVolumeMftData(PDWORD pErrorCode = nullptr);
private:
	PVOLUME_BPB_DATA m_MftData;  //NTFS_VOLUME_DATA_BUFFER;
	CLdString m_VolumeGuid;           //��GUID
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD m_FileSystemFlags;
	DWORD GetVolumeInfo();
private:  //�ӿں���
};
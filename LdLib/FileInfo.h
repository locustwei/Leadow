#pragma once
#include "ldstring.h"
#include "LdArray.h"
#include "VolumeUtils.h"

enum VF_FILE_TYPE
{
	vft_error,
	vft_file,
	vft_folder,
	vft_volume
};

class CVirtualFile
{
public:
	CVirtualFile() { m_Tag = 0; };
	virtual TCHAR* GetFullName() = 0;
	virtual TCHAR* GetFileName() = 0;
	virtual INT64 GetDataSize() = 0;
	virtual DWORD GetAttributes() = 0;
	virtual VF_FILE_TYPE GetFileType() = 0;
	virtual CLdArray<CVirtualFile*>* GetFiles() = 0;
	virtual CVirtualFile* GetFolder() = 0;
	UINT_PTR GetTag() { return m_Tag; };
	void SetTag(UINT_PTR Tag) { m_Tag = Tag; };
protected:
	UINT_PTR m_Tag;
};

class CFileInfo : public CVirtualFile
{
	friend class CFindFileCallbackImpl;
	friend class CFolderInfo;
public:
	CFileInfo();
	virtual ~CFileInfo();

	//���Ķ���ָ���ļ��ļ�
	virtual bool SetFileName(TCHAR* pFileName);
	bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
	//��Ŀ¼
	CVirtualFile* GetFolder() override;
	//Ŀ¼�µ��ļ�
	CLdArray<CVirtualFile*>* GetFiles() override { return nullptr; };
	//�ļ�����-----------------------------------------------------------------------------
	TCHAR* GetFullName() override;
	TCHAR* GetFileName() override;
	FILETIME GetCreateTime();
	FILETIME GetLastWriteTime();
	FILETIME GetLastAccessTime();
	INT64 GetDataSize() override;
	DWORD GetAttributes() override;
	VF_FILE_TYPE GetFileType() override { return vft_file; };
protected:
	typedef struct FILE_ATTRIBUTE_DATA {
		DWORD dwFileAttributes;
		FILETIME ftCreationTime;
		FILETIME ftLastAccessTime;
		FILETIME ftLastWriteTime;
		UINT64 nFileSize;
	} *PFILE_ATTRIBUTE_DATA;

	FILE_ATTRIBUTE_DATA m_AttributeData;
	CLdString m_FileName;
	CFileInfo* m_Folder;

	void ClearValue();
	bool GetFileBasicInfo();
};

class CFolderInfo: public CFileInfo
{
	friend class CFindFileCallbackImpl;
public:
	CFolderInfo();
	//ö��Ŀ¼�µ����ļ�
	//if bTree ��ݹ�ö����Ŀ¼�ļ�
	DWORD FindFiles(bool bTree = false);
	//Ŀ¼�µ��ļ�
	CLdArray<CVirtualFile*>* GetFiles() override;
	int AddFile(CFileInfo* pFile);
	//���ļ���
	UINT GetFilesCount() const;
	//
	CFileInfo* Find(TCHAR* pName, bool bSub = false);
	void Sort();

	VF_FILE_TYPE GetFileType() override { return vft_folder; };
private:
	CLdArray<CVirtualFile*> m_Files;
	int AddFile(PWIN32_FIND_DATA pData);
};

class CVolumeInfo: public CFolderInfo
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
	bool SetFileName(TCHAR* pFileName) override; //��·�����̷������򿪾���
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
	DWORD GetTotalClusters(PUINT64 pOut);
	//������Ϊ�û�����Ŀռ䣩
	BOOL HasQuota();
	//�Ƿ�����̷�
	BOOL IsMounted();
	//����MFT��ʽ��
	DWORD GetFileSystem(VOLUME_FILE_SYSTEM* pOut);
	//��ѯ�������� IOCTL_DISK_PERFORMANCE
	//������ϢIOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle() const;             //CreateFile��
	VF_FILE_TYPE GetFileType() override { return vft_volume; };

	DWORD GetBytesPerFileRecord(PDWORD pOut);
	DWORD GetMftValidSize(PUINT64 pOut);
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
	CLdString m_VolumeGuid;           //��GUID
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD m_FileSystemFlags;
	DWORD GetVolumeInfo();
	DWORD GetVolumeMftData();
private:  //�ӿں���
};


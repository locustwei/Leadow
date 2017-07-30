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

	//更改对象指向文件文件
	virtual bool SetFileName(TCHAR* pFileName);
	bool SetFindData(const TCHAR* pPath, PWIN32_FIND_DATA pData);
	//父目录
	CVirtualFile* GetFolder() override;
	//目录下的文件
	CLdArray<CVirtualFile*>* GetFiles() override { return nullptr; };
	//文件属性-----------------------------------------------------------------------------
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
	//枚举目录下的子文件
	//if bTree 则递归枚举子目录文件
	DWORD FindFiles(bool bTree = false);
	//目录下的文件
	CLdArray<CVirtualFile*>* GetFiles() override;
	int AddFile(CFileInfo* pFile);
	//子文件数
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
	// Qualifier:  初始化磁盘
	// Parameter: TCHAR * pGuid  磁盘“名称”用findvolumen返回的GUID。
	//************************************
	DWORD  OpenVolume(TCHAR* pGuid);
	TCHAR*  GetVolumeGuid();
	//************************************/*
	// Qualifier: 初始化磁盘
	// Parameter: const TCHAR * pPath 盘符（也可能是映射路径）
	//************************************
	bool SetFileName(TCHAR* pFileName) override; //卷路径（盘符），打开卷句柄
	//可用空闲空间
	DWORD GetAvailableFreeSpace(PUINT64 pOut);
	//磁盘空闲空间（可能大于可用空间）
	DWORD GetTotalFreeSpace(PUINT64 pOut);
	//总空间
	DWORD GetTotalSize(PUINT64 pOut);
	//Cluster 字节数
	DWORD GetClusterSize(PUINT pOut);
	//Sector 字节数
	DWORD GetSectorSize(PUINT pOut);
	//Cluter 数
	DWORD GetTotalClusters(PUINT64 pOut);
	//磁盘配额（为用户分配的空间）
	BOOL HasQuota();
	//是否分配盘符
	BOOL IsMounted();
	//磁盘MFT格式。
	DWORD GetFileSystem(VOLUME_FILE_SYSTEM* pOut);
	//查询磁盘性能 IOCTL_DISK_PERFORMANCE
	//分区信息IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS

	HANDLE OpenVolumeHandle() const;             //CreateFile，
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
	CLdString m_VolumeGuid;           //卷GUID
	VOLUME_FILE_SYSTEM m_FileSystemName;
	DWORD m_FileSystemFlags;
	DWORD GetVolumeInfo();
	DWORD GetVolumeMftData();
private:  //接口函数
};


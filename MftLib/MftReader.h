/*!
 * author asa
 *
 * 文件分配表直接读取抽象Class
 * 1、分析文件分配表MFT，读取磁盘文件信息
 * 2、数据更新
 *
 * FAT（16、32）、NTFS读取对象继承此Class
 *
*/
#pragma once

#define FILE_ATTRIBUTE_DELETED 0x80000000
//文件信息
typedef struct LDLIB_API _FILE_INFO{
	UINT64 DirectoryFileReferenceNumber;         //
	_FILETIME CreationTime;       // Saved when filename last changed
	_FILETIME ChangeTime;         //
	_FILETIME LastWriteTime;      //
	_FILETIME LastAccessTime;     //
	UINT64 AllocatedSize;      //
	UINT64 DataSize;           //
	DWORD FileAttributes;         //
	UCHAR NameLength;             //
	TCHAR Name[1];                //
}FILE_INFO, *PFILE_INFO;

//使用者回掉接口
__interface IMftReadeHolder
{
	//枚举文件回掉
	virtual BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) = 0;
	//文件更新回掉
	//virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param) = 0;
};

class LDLIB_API CMftReader
{
public:
	IMftReadeHolder* SetHolder(IMftReadeHolder* pHolder); //设置回掉接口

	virtual UINT64 EnumFiles(UINT_PTR Param) = 0;    //读取MFT文件
	//virtual const PFILE_INFO GetFileInfo(UINT64 ReferenceNumber);  //读取文件信息，文件序号
	//virtual USN GetLastUsn();      //获取MFT当前标识，用于更新文件时传入
	//virtual USN UpdateFiles(USN LastUsn, PVOID param); //更新文件信息

	//BOOL IsValid();     //

	static CMftReader* CreateReader(CVolumeInfo * pVolume);
public:
	CMftReader(void);
	virtual ~CMftReader(void);
protected:
	HANDLE m_Handle;
	IMftReadeHolder* m_Holder;
	CVolumeInfo * m_Volume;
	PVOLUME_BPB_DATA m_BpbData;

	BOOL ReadSector(UINT64 sector, DWORD count, PVOID buffer);
	BOOL Callback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param);

	virtual void ZeroMember();
};

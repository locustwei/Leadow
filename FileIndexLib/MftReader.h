/*!
 * author asa
 *
 * 分区文件分配表直接读取抽象Class
 * 1、分析文件分配表MFT，读取磁盘文件信息
 * 2、数据更新
 *
 * FAT（16、32）、NTFS读取对象继承此Class
 *
*/
#pragma once

#include <WinIoCtl.h>

//文件信息
typedef struct _FILE_INFO{
// 	union{
// 		struct{
// 			USHORT nVolume;
// 			ULONGLONG DirectoryReferenceNumber :48;
// 		};
// 		ULONGLONG DirectoryFileReferenceNumber;  //0x00000004
// 	};
	ULONGLONG DirectoryFileReferenceNumber;         //
	_FILETIME CreationTime;       // Saved when filename last changed
	_FILETIME ChangeTime;         //
	_FILETIME LastWriteTime;      //
	_FILETIME LastAccessTime;     //
	ULONGLONG AllocatedSize;      //
	ULONGLONG DataSize;           //
	ULONG FileAttributes;         //
	UCHAR NameLength;             //
	WCHAR Name[1];                //
}FILE_INFO, *PFILE_INFO;

//使用者回掉接口
typedef struct IMftReadeHolder{
	//枚举文件回掉
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param) = 0; 
	//文件更新回掉
	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param) = 0;
}*PMftReadeHolder;

class CMftReader
{
public:
	BOOL OpenVolume(const wchar_t* wsz_volum); //打开盘符
	BOOL SetHandle(HANDLE hVolume);            //已有Handle
	PMftReadeHolder SetHolder(PMftReadeHolder pHolder); //设置回掉接口

	virtual ULONGLONG EnumFiles(PVOID Param);    //读取MFT文件
	virtual const PFILE_INFO GetFileInfo(ULONGLONG ReferenceNumber);  //读取文件信息，文件序号
	virtual USN GetLastUsn();      //获取MFT当前标识，用于更新文件时传入
	virtual USN UpdateFiles(USN LastUsn, PVOID param); //更新文件信息

	BOOL IsValid();     //

	static CMftReader* CreateReader(HANDLE hVolume);
public:
	CMftReader(void);
	virtual ~CMftReader(void);
protected:
	//PUCHAR m_Bpb;
	USHORT m_BytesPerSector;
	USHORT m_SectorsPerCluster;
	HANDLE m_Handle, m_hVolumeBack;
	PMftReadeHolder m_Holder;
	//PVOID m_Param;
	stringxw m_wstr_volum;

	virtual bool Init();
	BOOL ReadSector(ULONGLONG sector, ULONG count, PVOID buffer);
	BOOL Callback(ULONGLONG ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param);

	virtual void ZeroMember();
};


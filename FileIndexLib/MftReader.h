/*!
 * author asa
 *
 * �����ļ������ֱ�Ӷ�ȡ����Class
 * 1�������ļ������MFT����ȡ�����ļ���Ϣ
 * 2�����ݸ���
 *
 * FAT��16��32����NTFS��ȡ����̳д�Class
 *
*/
#pragma once

#include <WinIoCtl.h>

//�ļ���Ϣ
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

//ʹ���߻ص��ӿ�
typedef struct IMftReadeHolder{
	//ö���ļ��ص�
	virtual BOOL EnumMftFileCallback(ULONGLONG ReferenceNumber, PFILE_INFO pFileInfo, PVOID Param) = 0; 
	//�ļ����»ص�
	virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param) = 0;
}*PMftReadeHolder;

class CMftReader
{
public:
	BOOL OpenVolume(const wchar_t* wsz_volum); //���̷�
	BOOL SetHandle(HANDLE hVolume);            //����Handle
	PMftReadeHolder SetHolder(PMftReadeHolder pHolder); //���ûص��ӿ�

	virtual ULONGLONG EnumFiles(PVOID Param);    //��ȡMFT�ļ�
	virtual const PFILE_INFO GetFileInfo(ULONGLONG ReferenceNumber);  //��ȡ�ļ���Ϣ���ļ����
	virtual USN GetLastUsn();      //��ȡMFT��ǰ��ʶ�����ڸ����ļ�ʱ����
	virtual USN UpdateFiles(USN LastUsn, PVOID param); //�����ļ���Ϣ

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


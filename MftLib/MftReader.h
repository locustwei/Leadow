/*!
 * author asa
 *
 * �ļ������ֱ�Ӷ�ȡ����Class
 * 1�������ļ������MFT����ȡ�����ļ���Ϣ
 * 2�����ݸ���
 *
 * FAT��16��32����NTFS��ȡ����̳д�Class
 *
*/
#pragma once

#define FILE_ATTRIBUTE_DELETED 0x80000000
//�ļ���Ϣ
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

//ʹ���߻ص��ӿ�
__interface IMftReadeHolder
{
	//ö���ļ��ص�
	virtual BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) = 0;
	//�ļ����»ص�
	//virtual BOOL EnumUsnRecordCallback(PUSN_RECORD record, PVOID Param) = 0;
};

class LDLIB_API CMftReader
{
public:
	IMftReadeHolder* SetHolder(IMftReadeHolder* pHolder); //���ûص��ӿ�

	virtual UINT64 EnumFiles(UINT_PTR Param) = 0;    //��ȡMFT�ļ�
	//virtual const PFILE_INFO GetFileInfo(UINT64 ReferenceNumber);  //��ȡ�ļ���Ϣ���ļ����
	//virtual USN GetLastUsn();      //��ȡMFT��ǰ��ʶ�����ڸ����ļ�ʱ����
	//virtual USN UpdateFiles(USN LastUsn, PVOID param); //�����ļ���Ϣ

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

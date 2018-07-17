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

//�ļ���Ϣ
typedef struct MFT_FILE_DATA
{
	UINT64 FileReferenceNumber;         //0x00000001
	UINT64 DirectoryFileReferenceNumber;         //
	_FILETIME CreationTime;       // Saved when filename last changed
								  //_FILETIME ChangeTime;         //
	_FILETIME LastWriteTime;      //
								  //_FILETIME LastAccessTime;     //
								  //UINT64 AllocatedSize;      //
	UINT64 DataSize;           //
	ULONG FileAttributes;         //
	UCHAR NameLength;             //
	WCHAR Name[MAX_PATH + 1];                //
}*PMFT_FILE_DATA;

class CMftReader;

class CMftFile
{
public:
	CMftFile(CMftReader* reader)
	{
		m_Reader = reader;
	};
	virtual ~CMftFile() {};
	virtual PMFT_FILE_DATA GetFileData() = 0;
protected:
	CMftReader* m_Reader;
};

//ʹ���߻ص��ӿ�
interface IMftReaderHandler
{
	virtual BOOL EnumMftFileCallback(PMFT_FILE_DATA pFileInfo, PVOID Param) = 0; 
};

class CMftReader
{
public:
	virtual UINT64 EnumFiles(IMftReaderHandler*, PVOID);
	virtual UINT64 EnumDeleteFiles(IMftReaderHandler*, PVOID);
	virtual BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) = 0;  //��ȡ�ļ���Ϣ���ļ����
	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = true) = 0;

	BOOL IsValid();     
public:
	CMftReader(HANDLE hVolume);
	virtual ~CMftReader(void);
protected:
	USHORT m_BytesPerSector;
	USHORT m_SectorsPerCluster;
	HANDLE m_Handle;
	IMftReaderHandler* m_Holder;
	bool m_Inited;
	PVOID m_Param;
	bool m_IsDeleted;
	BOOL ReadSector(UINT64 sector, ULONG count, PVOID buffer);
	BOOL Callback(UINT64 ReferenceNumber, PMFT_FILE_DATA pFileInfo);

	virtual void ZeroMember();
	virtual bool Init();

};

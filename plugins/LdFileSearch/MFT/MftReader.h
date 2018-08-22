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

//文件信息
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
	UINT FileAttributes;         //
	UCHAR NameLength;             //
	WCHAR Name[MAX_PATH + 1];                //
}*PMFT_FILE_DATA;

class CMftReader;


typedef struct LCN_BLOCK    //逻辑组块
{
	UINT64 startLcn; //起始组
	UINT nCount;     //连续组数
}*PLCN_BLOCK;

typedef struct FILE_DATA_STREAM
{
	UINT64 Size;         //数据流大小
	PUINT64 Lcns;
	UINT LcnCount;
	UINT Offset;       //属性相对于文件记录的偏移位置(值为0时是Nonresident)
}*PFILE_DATA_STREAM;

class CMftFile
{
public:
	CMftFile(CMftReader* reader)
	{
		m_Reader = reader;
	};
	virtual ~CMftFile() {};

	PMFT_FILE_DATA GetFileData()
	{
		return &m_FileInfo;
	}
protected:
	MFT_FILE_DATA m_FileInfo;           //标准属性
	CMftReader* m_Reader;
};

//使用者回掉接口
interface IMftReaderHandler
{
	virtual BOOL EnumMftFileCallback(PMFT_FILE_DATA, PVOID) = 0; 
};

interface IMftDeleteReaderHandler
{
	virtual BOOL EnumMftDeleteFileCallback(PMFT_FILE_DATA, PFILE_DATA_STREAM, PVOID) = 0;
};

class CMftReader
{
public:
	virtual UINT64 EnumFiles(IMftReaderHandler*, PVOID);
	virtual UINT64 EnumDeleteFiles(IMftDeleteReaderHandler*, PVOID);
	virtual BOOL GetFileInfo(UINT64 ReferenceNumber, PMFT_FILE_DATA aFileInfo) = 0;  //读取文件信息，文件序号
	virtual CMftFile* GetFile(UINT64 FileNumber, bool OnlyName = true) = 0;

	BOOL ReadSector(UINT64 sector, UINT count, PVOID buffer);
	virtual BOOL ReadCluster(UINT64 Cluster, UINT count, PVOID buffer);

	USHORT GetBytesPerSector();
	USHORT GetSectorsPerCluster();
	UINT64 GetTotalCluster();

	BOOL IsValid(); 

	static CMftReader* CreateReader(HANDLE hVolume, UINT fs);
public:
	CMftReader(HANDLE hVolume);
	virtual ~CMftReader(void);
protected:
	USHORT m_BytesPerSector;
	USHORT m_SectorsPerCluster;
	UINT64 m_TotalCluster;

	HANDLE m_Handle;
	IMftReaderHandler* m_Holder;
	IMftDeleteReaderHandler* m_DelHolder;
	bool m_Inited;
	PVOID m_Param;
	bool m_IsDeleted;
	BOOL Callback(UINT64 ReferenceNumber, PMFT_FILE_DATA pFileInfo);
	BOOL DelCallback(UINT64 ReferenceNumber, PMFT_FILE_DATA pFileInfo, PFILE_DATA_STREAM);

	virtual void ZeroMember();
	virtual bool Init();
};

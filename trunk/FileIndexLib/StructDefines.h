#pragma once

/*
* ת���ļ���¼����
*/
typedef enum RECORD_FIELD_CLASS {
	RFC_NONE = 0x00000000,
	RFC_FN = 0x00000001,  //FileReferenceNumber
	RFC_DIR = 0x00000002,  //IsDir
	RFC_DRN = 0x00000004,  //DirectoryFileReferenceNumber
	RFC_CRT = 0x00000008,  //CreationTime
	RFC_CHT = 0x00000010,  //ChangeTime
	RFC_LWT = 0x00000020,  //LastWriteTime
	RFC_LAT = 0x00000040,  //LastAccessTime
	RFC_DS = 0x00000080,  //DataSize
	RFC_NL = 0x00000100,  //NameLength
	RFC_MD5 = 0x00000200,  //MD5
	RFC_NAME = 0x00000400   //FileName
};

typedef struct FILTER_DIRECTORY {
	BOOL sub;
	ULONGLONG DircetoryReference;
}*PFILTER_DIRECTORY;

/*
* �����ļ�����Χ��������
*/
typedef struct FILE_FILTER {
	PCWSTR* pIncludeNames;                           //�ļ������� 
	PCWSTR* pExcludeNames;                           //�ļ���������
	PFILTER_DIRECTORY* ParentDircetoryReferences;    //����Ŀ¼
	LONGLONG beginSize;                              //�ļ���С >=
	LONGLONG endSize;                                //�ļ���С <=
	BOOL onlyFile;                                   //����Ҫ�ļ�
}*PFILE_FILTER;

typedef struct ENUM_FILERECORD_PARAM {
	RECORD_FIELD_CLASS* sortField;                   //�������
	FILE_FILTER Filter;
}*PENUM_FILERECORD_PARAM;

#pragma pack(push, 1)

typedef struct MFTFILERECORD {                //ת���ļ���¼�ṹ
											  //ULONGLONG FileReferenceNumber;         //0x00000001
	bool IsDir;                              //0x00000002
	ULONGLONG DirectoryFileReferenceNumber;
	//	ULONGLONG CreationTime;              //0x00000008 
	//	ULONGLONG ChangeTime;                //0x00000010
	//	ULONGLONG LastWriteTime;             //0x00000020
	//	ULONGLONG LastAccessTime;            //0x00000040
	ULONGLONG DataSize;                      //0x00000080
	UCHAR NameLength;                        //0x00000100
	BYTE MD5[16];                  //0x00000200
	WCHAR Name[MAX_PATH + 1];                //0x00000400
}*PMFTFILERECORD;

#pragma pack(pop)


class IVolumeInterface;
//ʹ���߻ص��ӿ�
class WJS_API IVolumeEvent {
public:
	virtual VOID __stdcall OnSearchCompeleted(IVolumeInterface*) = 0;
	virtual VOID __stdcall OnRenewDump(IVolumeInterface*) = 0;
	virtual VOID __stdcall OnFindDupCompeleted(IVolumeInterface*) = 0;
};

//���ڶ�̬�⵼���ӿڡ�
class WJS_API IVolumeInterface {
public:
	virtual BOOL __stdcall OpenVolume(PWSTR) = 0;
	virtual PCWSTR __stdcall GetVolumeGuid() = 0;
	virtual BOOL __stdcall OpenVolumePath(const PWSTR wsz_volume) = 0;
	virtual PCWSTR __stdcall GetVolumePath() = 0;
	virtual BOOL __stdcall SetDumpFilePath(PCWSTR wsz_path) = 0;
	virtual BOOL __stdcall UnLoadDumpFile() = 0;
	virtual PCWSTR __stdcall GetDumpFileName() = 0;
	virtual VOID __stdcall SetHolper(IVolumeEvent* pHoler) = 0;

	virtual BOOL __stdcall UpdateMftDump(BOOL async) = 0;
	virtual BOOL __stdcall ListenFileChange() = 0;

	virtual BOOL __stdcall SearchFile(PFILE_FILTER pFilter, BOOL asyn) = 0;
};

class WJS_API IWJLibInterface {
public:
	virtual UINT __stdcall GetVolumeCount() = 0;
	virtual IVolumeInterface* __stdcall GetVolume(ULONG idx) = 0;
	virtual BOOL __stdcall SetDumpFilePath(PCWSTR wsz_path) = 0;      //����ת���ļ����·��
};

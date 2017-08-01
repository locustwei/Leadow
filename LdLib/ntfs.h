#pragma once

typedef struct _NTFS_RECORD_HEADER{
    DWORD Type;  //4B??'ELIF','XDNI','DAAB','ELOH','DKHC'????
    USHORT UsaOffset; //2B?更新序列号数组偏移，校验值地址
    USHORT UsaCount; //1+n?1为校验值个数?n为待替换值个数??fixup?
    USN Usn;
} NTFS_RECORD_HEADER, *PNTFS_RECORD_HEADER;

typedef struct _FILE_RECORD_HEADER{
    NTFS_RECORD_HEADER Ntfs;
    USHORT SequenceNumber;
    USHORT LinkCount;
    USHORT AttributesOffset;
    USHORT Flags; // 0x0001 = InUse, 0x0002= Directory
    DWORD BytesInUse;
    DWORD BytesAllocated;
    UINT64 BaseFileRecord;
    USHORT NextAttributeNumber;
} FILE_RECORD_HEADER, *PFILE_RECORD_HEADER;

typedef enum {
    AttributeStandardInformation = 0x10,
    AttributeAttributeList = 0x20,
    AttributeFileName = 0x30,
    AttributeObjectId = 0x40,
    AttributeSecurityDescriptor = 0x50,
    AttributeVolumeName = 0x60,
    AttributeVolumeInformation = 0x70,
    AttributeData = 0x80,
    AttributeIndexRoot = 0x90,
    AttributeIndexAllocation = 0xA0,
    AttributeBitmap = 0xB0,
    AttributeReparsePoint = 0xC0,
    AttributeEAInformation = 0xD0,
    AttributeEA = 0xE0,
    AttributePropertySet = 0xF0,
    AttributeLoggedUtilityStream = 0x100
} ATTRIBUTE_TYPE, *PATTRIBUTE_TYPE;

typedef struct _ATTRIBUTE{
    ATTRIBUTE_TYPE AttributeType;
    DWORD Length;
    BOOLEAN Nonresident;
    UCHAR NameLength;
    USHORT NameOffset;
    USHORT Flags; // 0x0001 = Compressed
    USHORT AttributeNumber;
} ATTRIBUTE, *PATTRIBUTE;

typedef struct _RESIDENT_ATTRIBUTE{
    ATTRIBUTE Attribute;
    DWORD ValueLength;
    USHORT ValueOffset;
    USHORT Flags; // 0x0001 = Indexed
} RESIDENT_ATTRIBUTE, *PRESIDENT_ATTRIBUTE;

typedef struct _NONRESIDENT_ATTRIBUTE{
    ATTRIBUTE Attribute;
    UINT64 LowVcn;
    UINT64 HighVcn;
    USHORT RunArrayOffset;
    UCHAR CompressionUnit;
    UCHAR AlignmentOrReserved[5];
    UINT64 AllocatedSize;
    UINT64 DataSize;
    UINT64 InitializedSize;
    UINT64 CompressedSize; // Only when compressed
} NONRESIDENT_ATTRIBUTE, *PNONRESIDENT_ATTRIBUTE;

typedef struct _STANDARD_INFORMATION{
    _FILETIME CreationTime;
    _FILETIME ChangeTime;
    _FILETIME LastWriteTime;
    _FILETIME LastAccessTime;
    DWORD FileAttributes;
    DWORD AlignmentOrReservedOrUnknown[3];
    DWORD QuotaId;                // NTFS 3.0
    DWORD SecurityId;       // NTFS 3.0
    UINT64 QuotaCharge;  // NTFS 3.0
    USN Usn;                      // NTFS 3.0
} STANDARD_INFORMATION, *PSTANDARD_INFORMATION;

typedef struct _ATTRIBUTE_LIST{
    ATTRIBUTE_TYPE AttributeType;
    USHORT Length;
    UCHAR NameLength;
    UCHAR NameOffset;
    UINT64 StartVcn;
    UINT64 FileReferenceNumber;
    USHORT AttributeNumber;
    USHORT AlignmentOrReserved[3];
} ATTRIBUTE_LIST, *PATTRIBUTE_LIST;

typedef struct _FILENAME_ATTRIBUTE{
// 	union{
// 		struct{
// 			USHORT nVolume;
// 			UINT64 DirectoryReferenceNumber :48;
// 		};
// 		UINT64 DirectoryFileReferenceNumber;  //0x00000004
// 	};
	UINT64 DirectoryFileReferenceNumber;         //
    _FILETIME CreationTime;       // Saved when filename last changed
    _FILETIME ChangeTime;         //
    _FILETIME LastWriteTime;      //
    _FILETIME LastAccessTime;     //
    UINT64 AllocatedSize;      //
    UINT64 DataSize;                 //
    DWORD FileAttributes;         //
    DWORD AlignmentOrReserved;    //
    UCHAR NameLength;             //
    UCHAR NameType;                     // 0x01 = Long, 0x02 = Short
    TCHAR Name[1];                      //
} FILENAME_ATTRIBUTE, *PFILENAME_ATTRIBUTE;

typedef struct _OBJECTID_ATTRIBUTE{
    GUID ObjectId;
    union {
        struct {
            GUID BirthVolumeId;
            GUID BirthObjectId;
            GUID DomainId;
        };
        UCHAR ExtendedInfo[48];
    };
} OBJECTID_ATTRIBUTE, *POBJECTID_ATTRIBUTE;

typedef struct _VOLUME_INFORMATION{
    DWORD Unknown[2];
    UCHAR MajorVersion;
    UCHAR MinorVersion;
    USHORT Flags;
} VOLUME_INFORMATION, *PVOLUME_INFORMATION;

typedef struct _DIRECTORY_INDEX{
    DWORD EntriesOffset;
    DWORD IndexBlockLength;
    DWORD AllocatedSize;
    DWORD Flags; // 0x00 = Small directory, 0x01 = Large directory
} DIRECTORY_INDEX, *PDIRECTORY_INDEX;

typedef struct _DIRECTORY_ENTRY{
    UINT64 FileReferenceNumber;
    USHORT Length;
    USHORT AttributeLength;
    DWORD Flags; // 0x01 = Has trailing VCN, 0x02 = Last entry
    // FILENAME_ATTRIBUTE Name;
    // UINT64 Vcn; // VCN in IndexAllocation of earlier entries
} DIRECTORY_ENTRY, *PDIRECTORY_ENTRY;

typedef struct _INDEX_ROOT{
    ATTRIBUTE_TYPE Type;
    DWORD CollationRule;
    DWORD BytesPerIndexBlock;
    DWORD ClustersPerIndexBlock;
    DIRECTORY_INDEX DirectoryIndex;
} INDEX_ROOT, *PINDEX_ROOT;

typedef struct _INDEX_BLOCK_HEADER{
    NTFS_RECORD_HEADER Ntfs;
    UINT64 IndexBlockVcn;
    DIRECTORY_INDEX DirectoryIndex;
} INDEX_BLOCK_HEADER, *PINDEX_BLOCK_HEADER;

typedef struct _REPARSE_POINT{
    DWORD ReparseTag;
    USHORT ReparseDataLength;
    USHORT Reserved;
    UCHAR ReparseData[1];
} REPARSE_POINT, *PREPARSE_POINT;

typedef struct _EA_INFORMATION{
    DWORD EaLength;
    DWORD EaQueryLength;
} EA_INFORMATION, *PEA_INFORMATION;

typedef struct _EA_ATTRIBUTE{
    DWORD NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];
    // UCHAR EaData[];
} EA_ATTRIBUTE, *PEA_ATTRIBUTE;

typedef struct _ATTRIBUTE_DEFINITION{
    TCHAR AttributeName[64];
    DWORD AttributeNumber;
    DWORD Unknown[2];
    DWORD Flags;
    UINT64 MinimumSize;
    UINT64 MaximumSize;
} ATTRIBUTE_DEFINITION, *PATTRIBUTE_DEFINITION;

#pragma pack(push, 1)

typedef struct _NTFS_BPB{
    UCHAR Jump[3];
    UCHAR Format[8];
    USHORT BytesPerSector;
    UCHAR SectorsPerCluster;
    USHORT BootSectors;
    UCHAR Mbz1;
    USHORT Mbz2;
    USHORT Reserved1;
    UCHAR MediaType;
    USHORT Mbz3;
    USHORT SectorsPerTrack;
    USHORT NumberOfHeads;
    DWORD PartitionOffset;
    DWORD Reserved2[2];
    UINT64 TotalSectors;
    UINT64 MftStartLcn;
    UINT64 Mft2StartLcn;
    DWORD ClustersPerFileRecord;
    DWORD ClustersPerIndexBlock;
    LARGE_INTEGER VolumeSerialNumber;
    DWORD CheckSum;
    UCHAR Code[0x1aa];
    USHORT BootSignature;
} NTFS_BPB, *PNTFS_BPB;

#pragma pack(pop)
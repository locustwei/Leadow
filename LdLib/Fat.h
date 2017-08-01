#pragma once

#define DELETE_FLAG              0xE5

typedef enum FAT_FILE_ATTRIBUTE{
	FFT_READONLY  = 0x01,
	FFT_HIDE      = 0x02,
	FFT_SYSTEM    = 0x04,
	FFT_VOLUME    = 0x08,
	FFT_LONGNAME  = 0x0F,
	FFT_DIRECTORY = 0x10,
	FFT_DOC       = 0x20
};

typedef enum CLUSTER_NUMBER_12{
	EOC12 = 0x0FFF,
	BAD12 = 0x0FF7,
	RES12 = 0x0FF8,
};

typedef enum CLUSTER_NUMBER_16{
	EOC16 = 0xFFFF,
	BAD16 = 0xFFF7,
	RES16 = 0xFFF8,
};

typedef enum CLUSTER_NUMBER_32{
	EOC32 = 0x0FFFFFFF,
	BAD32 = 0x0FFFFFF7,
	RES32 = 0x0FFFFFF8,
};

#pragma pack(push, 1)

typedef struct FAT_BPB {
	BYTE JmpBoot [ 3 ];         //Jump instruction   //跳转指令（跳过开头一段区域）
	char OEMName [ 8 ];         //OEM名称（空格补齐）。MS-DOS检查这个区域以确定使用启动记录中的哪一部分数据[3] 。常见值是IBM 3.3（在“IBM”和“3.3”之间有两个空格）和MSDOS5.0.
	WORD BytesPerSector;        //每个扇区的字节数。基本输入输出系统参数块从这里开始。
	BYTE SectorsPerCluster;     //每簇扇区数
	WORD ReservedSectors;       //保留扇区数（包括启动扇区）
	BYTE NumFATs;               //FAT个数
	WORD RootEntriesCount;      //最大根目录条目个数(FAT12、FAT16使用)
	WORD TotalSectors16;         //扇区数(FAT12、FAT16使用，FAT32该值是0）
	BYTE Media;                 //Media identifier
	WORD SectorsPerFAT16;       //每个文档分配表的扇区数（FAT16使用）
	WORD SecPerTrk;             //每磁道的扇区
	WORD NumHeads;              //磁头数
	DWORD HiddenSectors;        //分区前已使用扇区数(MBR)
	DWORD TotalSectors32;       //总扇区数（如果超过65535，参见偏移0x13）
	union {
		struct {
			BYTE _DrvNum;          //物理驱动器个数(FAT16)
			BYTE _Reserved1;      //当前磁头(FAT16)
			BYTE _BootSig;        //签名(FAT16)
			DWORD _VolID;         //ID（FAT16）
			char _ValLab [ 11 ];   //Volume label
			char FilSysType [ 8 ]; //File system name"FAT12" "FAT16"
		}FAT1216;
		// FAT32
		struct {
			DWORD SectorsPerFAT32;       //Sectors per one FAT
			WORD ExtFlags;               //扩展标志位0-3：活动 FAT数(从0开始计数，而不是1)；位4-6：保留；位7：0值意味着在运行时FAT被映射到所有的FAT，1值表示只有一个FAT是活动的；位8-15：保留
			WORD FSVer;                  //版本号
			DWORD RootClus;              //根目录起始簇
			WORD FSInfo;                 //该扇区为操作系统提供关于空簇总数及下一可用簇的信息
			WORD BkBootSec;              //备份引导扇区总是位于文件系统的 6 号扇区。 
			BYTE Reserved1 [ 12 ];       /* range 0..11*/ //Reserved
			BYTE DrvNum;                 //Drive number
			BYTE Reserved2;              //Reserved
			BYTE BootSig;                //Extended boot signature
			DWORD VolID;                 //Volume serial number
			char VolLab [ 11 ];          //Volume label
			char FilSysType [ 8 ];       //File system name
		}FAT32;
	};
} *PFAT_BPB;

typedef struct FAT_FILE{
	BYTE Order;          //短文件名第一个字符 或 长文件名序号 或 删除标志0xE5 或 未使用0）
	union{
		UINT64 ReferenceNumber;  //方便程序使用		
		CHAR Name1[10];
	};
	BYTE Attr;  //0x01-只读  0x02-隐藏  0x04-系统文件  0x08-卷标 0x0F-为此值时表示该目录项为长文件名目录项  0x10-目录 0x20-存档 
	BYTE Reserved1;
	union{
		struct{
// 			CHAR Name[8];
// 			CHAR ExtName[3];
			BYTE CreateTimeTength;
			WORD CreateTime;
			WORD CreateDate;
			WORD LastAccessDate;
			WORD ClusterNumberHigh;  //文件起始簇号的高两个字节。 
			WORD WriteTime;
			WORD WriteDate;
			WORD ClusterNumberLow; //文件内容起始簇号的低两个字节，
			DWORD FileSize;
		};  //短文件名项
		struct{
			BYTE Checksum;      //校验
			TCHAR Name2[6];
			WORD FirstCluster1;  //固定为0
			TCHAR Name3[2];
		}L; //长文件名项（以倒序排列在短文件名前）
	};
}*PFAT_FILE;

#pragma pack(pop)

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
	UCHAR JmpBoot [ 3 ];         //Jump instruction   //跳转指令（跳过开头一段区域）
	char OEMName [ 8 ];         //OEM名称（空格补齐）。MS-DOS检查这个区域以确定使用启动记录中的哪一部分数据[3] 。常见值是IBM 3.3（在“IBM”和“3.3”之间有两个空格）和MSDOS5.0.
	union
	{
		struct
		{    //exFAT结构
			UCHAR Exfat_Reserved[0x35];
			UINT64 HiddenSectors;        //隐藏扇区数
			UINT64 TotalSectors;         //分区总扇区数
			UINT FATStartSector;         //FAT表起始扇区号
			UINT FATSectors;             //FAT表扇区数
			UINT FirstClusterSector;     //首簇起始扇区号
			UINT ToltalClusters;         //分区内的总簇数
			UINT RootClus;              //根目录起始簇
			UINT SeriNumber;            //卷序列号
			USHORT SeriVer;             //卷版本号
			USHORT VolumeFlag;          //卷标志
			UCHAR BytesPerSector;       //每个扇区的字节数。2^n。
			UCHAR SectorsPerCluster;     //每簇扇区数。2^n
			UCHAR NumFats;              //FAT个数
			UCHAR DiskFlag;             //驱动标记
			UCHAR UsedPersent;          // 分区使用百分比
		}EXFAT;
		struct
		{  //FAT结构
			USHORT BytesPerSector;        //每个扇区的字节数。基本输入输出系统参数块从这里开始。
			UCHAR SectorsPerCluster;     //每簇扇区数
			USHORT ReservedSectors;       //保留扇区数（包括启动扇区）
			UCHAR NumFATs;               //FAT个数
			USHORT RootEntriesCount;      //最大根目录条目个数(FAT12、FAT16使用)
			USHORT TotalSectors16;         //扇区数(FAT12、FAT16使用，FAT32该值是0）
			UCHAR Media;                 //Media identifier
			USHORT SectorsPerFAT16;       //每个文档分配表的扇区数（FAT16使用）
			USHORT SecPerTrk;             //每磁道的扇区
			USHORT NumHeads;              //磁头数
			UINT HiddenSectors;        //分区前已使用扇区数(MBR)
			UINT TotalSectors32;       //总扇区数（如果超过65535，参见偏移0x13）
			union {
				struct {
					UCHAR _DrvNum;          //物理驱动器个数(FAT16)
					UCHAR _Reserved1;      //当前磁头(FAT16)
					UCHAR _BootSig;        //签名(FAT16)
					UINT _VolID;         //ID（FAT16）
					char _ValLab[11];   //Volume label
					char FilSysType[8]; //File system name"FAT12" "FAT16"
				}FAT1216;
				// FAT32
				struct {
					UINT SectorsPerFAT32;       //Sectors per one FAT
					USHORT ExtFlags;               //扩展标志位0-3：活动 FAT数(从0开始计数，而不是1)；位4-6：保留；位7：0值意味着在运行时FAT被映射到所有的FAT，1值表示只有一个FAT是活动的；位8-15：保留
					USHORT FSVer;                  //版本号
					UINT RootClus;              //根目录起始簇
					USHORT FSInfo;                 //该扇区为操作系统提供关于空簇总数及下一可用簇的信息
					USHORT BkBootSec;              //备份引导扇区总是位于文件系统的 6 号扇区。 
					UCHAR Reserved1[12];       /* range 0..11*/ //Reserved
					UCHAR DrvNum;                 //Drive number
					UCHAR Reserved2;              //Reserved
					UCHAR BootSig;                //Extended boot signature
					UINT VolID;                 //Volume serial number
					char VolLab[11];          //Volume label
					char FilSysType[8];       //File system name
				}FAT32;
			};
		};
	};

} *PFAT_BPB;

typedef struct FAT_FILE{
	UCHAR Order;          //短文件名第一个字符 或 长文件名序号 或 删除标志0xE5 或 未使用0）
	union{
		UINT64 ReferenceNumber;  //文件索引号（临时使用）
		CHAR Name1[10];
	};
	UCHAR Attr;  //0x01-只读  0x02-隐藏  0x04-系统文件  0x08-卷标 0x0F-为此值时表示该目录项为长文件名目录项  0x10-目录 0x20-存档 
	union {
		UCHAR Reserved1;
		UCHAR IsDeleted;   //已删除（临时使用）
	};
	union{
		struct{
// 			CHAR Name[8];
// 			CHAR ExtName[3];
			UCHAR CreateTimeTength;
			USHORT CreateTime;
			USHORT CreateDate;
			USHORT LastAccessDate;
			USHORT ClusterNumberHigh;  //文件起始簇号的高两个字节。 
			USHORT WriteTime;
			USHORT WriteDate;
			USHORT ClusterNumberLow; //文件内容起始簇号的低两个字节，
			UINT FileSize;
		};  //短文件名项
		struct{
			UCHAR Checksum;      //校验
			WCHAR Name2[6];
			USHORT FirstCluster1;  //固定为0
			WCHAR Name3[2];
		}L; //长文件名项（以倒序排列在短文件名前）
	};

}*PFAT_FILE;

typedef struct EXFAT_FILE {
	UCHAR type;         
	union
	{
		struct        //卷属性 (type == 0x83)
		{
			UCHAR Length;
			CHAR Label[22];
			UCHAR Reserved1[8];
		}VOLUME;
		struct         //簇位图 (type = 0x81)
		{
			UCHAR Reserved1[19];
			UINT StartCluster;
			UINT64 Size;
		}CLUBIT;

		struct         //目录属性1(type == 0x85)
		{
			UCHAR AttributeNum;     //附属目录项数();
			USHORT Checksum;         //校验
			UINT Attribute;         //文件属性 01:只读; 10000:子目录;10:隐藏;100000:存档;100:系统
			UINT CreateTime;        //文件创建时间
			UINT LastMotifyTime;    //文件最后修改时间
			UINT LastAccessTime;    //文件最后访问时间
			UCHAR CreateTimeMS;     //文件创建时间精确至10ms
			UCHAR Reserved1[3];
			UCHAR Reserved2[8];
		}FILE1;
		struct       //目录属性2(type == 0xC0)
		{
			UCHAR fraFlag;       //文件碎片标志;(01:有碎片；03：没有碎片）
			UCHAR Reserved1;
			UCHAR NameLength;    //文件名字符数（wchar）
			USHORT NamneHash;    //文件名Hash值
			UCHAR Reserved2[2];
			UINT64 Size;         //文件大小1
			UCHAR Reserved3[4];
			UINT ClusterNum;     //起始簇号
			UINT64 CompSize;        //压缩文件大小
		}FILE2;
		struct       //目录属性3(type == 0xC1)
		{
			UCHAR Reserved1;
			WCHAR Name[15];      //文件名
		}FILE3;

		struct 
		{
			UCHAR fraFlag;       //文件碎片标志;(01:有碎片；03：没有碎片）
			UINT64 ReferenceNumber;  //文件索引号（临时使用）
			UINT StartCluster;       //起始族
			UINT64 Size;
			bool IsDel;
		}CUSTOM;        //本程序使用
	};
}*PEXFAT_FILE;
#pragma pack(pop)

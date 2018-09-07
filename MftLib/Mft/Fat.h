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
	UCHAR JmpBoot [ 3 ];         //Jump instruction   //��תָ�������ͷһ������
	char OEMName [ 8 ];         //OEM���ƣ��ո��룩��MS-DOS������������ȷ��ʹ��������¼�е���һ��������[3] ������ֵ��IBM 3.3���ڡ�IBM���͡�3.3��֮���������ո񣩺�MSDOS5.0.
	union
	{
		struct
		{    //exFAT�ṹ
			UCHAR Exfat_Reserved[0x35];
			UINT64 HiddenSectors;        //����������
			UINT64 TotalSectors;         //������������
			UINT FATStartSector;         //FAT����ʼ������
			UINT FATSectors;             //FAT��������
			UINT FirstClusterSector;     //�״���ʼ������
			UINT ToltalClusters;         //�����ڵ��ܴ���
			UINT RootClus;              //��Ŀ¼��ʼ��
			UINT SeriNumber;            //�����к�
			USHORT SeriVer;             //��汾��
			USHORT VolumeFlag;          //���־
			UCHAR BytesPerSector;       //ÿ���������ֽ�����2^n��
			UCHAR SectorsPerCluster;     //ÿ����������2^n
			UCHAR NumFats;              //FAT����
			UCHAR DiskFlag;             //�������
			UCHAR UsedPersent;          // ����ʹ�ðٷֱ�
		}EXFAT;
		struct
		{  //FAT�ṹ
			USHORT BytesPerSector;        //ÿ���������ֽ����������������ϵͳ����������￪ʼ��
			UCHAR SectorsPerCluster;     //ÿ��������
			USHORT ReservedSectors;       //��������������������������
			UCHAR NumFATs;               //FAT����
			USHORT RootEntriesCount;      //����Ŀ¼��Ŀ����(FAT12��FAT16ʹ��)
			USHORT TotalSectors16;         //������(FAT12��FAT16ʹ�ã�FAT32��ֵ��0��
			UCHAR Media;                 //Media identifier
			USHORT SectorsPerFAT16;       //ÿ���ĵ���������������FAT16ʹ�ã�
			USHORT SecPerTrk;             //ÿ�ŵ�������
			USHORT NumHeads;              //��ͷ��
			UINT HiddenSectors;        //����ǰ��ʹ��������(MBR)
			UINT TotalSectors32;       //�����������������65535���μ�ƫ��0x13��
			union {
				struct {
					UCHAR _DrvNum;          //��������������(FAT16)
					UCHAR _Reserved1;      //��ǰ��ͷ(FAT16)
					UCHAR _BootSig;        //ǩ��(FAT16)
					UINT _VolID;         //ID��FAT16��
					char _ValLab[11];   //Volume label
					char FilSysType[8]; //File system name"FAT12" "FAT16"
				}FAT1216;
				// FAT32
				struct {
					UINT SectorsPerFAT32;       //Sectors per one FAT
					USHORT ExtFlags;               //��չ��־λ0-3��� FAT��(��0��ʼ������������1)��λ4-6��������λ7��0ֵ��ζ��������ʱFAT��ӳ�䵽���е�FAT��1ֵ��ʾֻ��һ��FAT�ǻ�ģ�λ8-15������
					USHORT FSVer;                  //�汾��
					UINT RootClus;              //��Ŀ¼��ʼ��
					USHORT FSInfo;                 //������Ϊ����ϵͳ�ṩ���ڿմ���������һ���ôص���Ϣ
					USHORT BkBootSec;              //����������������λ���ļ�ϵͳ�� 6 �������� 
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
	UCHAR Order;          //���ļ�����һ���ַ� �� ���ļ������ �� ɾ����־0xE5 �� δʹ��0��
	union{
		UINT64 ReferenceNumber;  //�ļ������ţ���ʱʹ�ã�
		CHAR Name1[10];
	};
	UCHAR Attr;  //0x01-ֻ��  0x02-����  0x04-ϵͳ�ļ�  0x08-��� 0x0F-Ϊ��ֵʱ��ʾ��Ŀ¼��Ϊ���ļ���Ŀ¼��  0x10-Ŀ¼ 0x20-�浵 
	union {
		UCHAR Reserved1;
		UCHAR IsDeleted;   //��ɾ������ʱʹ�ã�
	};
	union{
		struct{
// 			CHAR Name[8];
// 			CHAR ExtName[3];
			UCHAR CreateTimeTength;
			USHORT CreateTime;
			USHORT CreateDate;
			USHORT LastAccessDate;
			USHORT ClusterNumberHigh;  //�ļ���ʼ�غŵĸ������ֽڡ� 
			USHORT WriteTime;
			USHORT WriteDate;
			USHORT ClusterNumberLow; //�ļ�������ʼ�غŵĵ������ֽڣ�
			UINT FileSize;
		};  //���ļ�����
		struct{
			UCHAR Checksum;      //У��
			WCHAR Name2[6];
			USHORT FirstCluster1;  //�̶�Ϊ0
			WCHAR Name3[2];
		}L; //���ļ�����Ե��������ڶ��ļ���ǰ��
	};

}*PFAT_FILE;

typedef struct EXFAT_FILE {
	UCHAR type;         
	union
	{
		struct        //������ (type == 0x83)
		{
			UCHAR Length;
			CHAR Label[22];
			UCHAR Reserved1[8];
		}VOLUME;
		struct         //��λͼ (type = 0x81)
		{
			UCHAR Reserved1[19];
			UINT StartCluster;
			UINT64 Size;
		}CLUBIT;

		struct         //Ŀ¼����1(type == 0x85)
		{
			UCHAR AttributeNum;     //����Ŀ¼����();
			USHORT Checksum;         //У��
			UINT Attribute;         //�ļ����� 01:ֻ��; 10000:��Ŀ¼;10:����;100000:�浵;100:ϵͳ
			UINT CreateTime;        //�ļ�����ʱ��
			UINT LastMotifyTime;    //�ļ�����޸�ʱ��
			UINT LastAccessTime;    //�ļ�������ʱ��
			UCHAR CreateTimeMS;     //�ļ�����ʱ�侫ȷ��10ms
			UCHAR Reserved1[3];
			UCHAR Reserved2[8];
		}FILE1;
		struct       //Ŀ¼����2(type == 0xC0)
		{
			UCHAR fraFlag;       //�ļ���Ƭ��־;(01:����Ƭ��03��û����Ƭ��
			UCHAR Reserved1;
			UCHAR NameLength;    //�ļ����ַ�����wchar��
			USHORT NamneHash;    //�ļ���Hashֵ
			UCHAR Reserved2[2];
			UINT64 Size;         //�ļ���С1
			UCHAR Reserved3[4];
			UINT ClusterNum;     //��ʼ�غ�
			UINT64 CompSize;        //ѹ���ļ���С
		}FILE2;
		struct       //Ŀ¼����3(type == 0xC1)
		{
			UCHAR Reserved1;
			WCHAR Name[15];      //�ļ���
		}FILE3;

		struct 
		{
			UCHAR fraFlag;       //�ļ���Ƭ��־;(01:����Ƭ��03��û����Ƭ��
			UINT64 ReferenceNumber;  //�ļ������ţ���ʱʹ�ã�
			UINT StartCluster;       //��ʼ��
			UINT64 Size;
			bool IsDel;
		}CUSTOM;        //������ʹ��
	};
}*PEXFAT_FILE;
#pragma pack(pop)

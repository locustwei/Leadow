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
	BYTE JmpBoot [ 3 ];         //Jump instruction   //��תָ�������ͷһ������
	char OEMName [ 8 ];         //OEM���ƣ��ո��룩��MS-DOS������������ȷ��ʹ��������¼�е���һ��������[3] ������ֵ��IBM 3.3���ڡ�IBM���͡�3.3��֮���������ո񣩺�MSDOS5.0.
	WORD BytesPerSector;        //ÿ���������ֽ����������������ϵͳ����������￪ʼ��
	BYTE SectorsPerCluster;     //ÿ��������
	WORD ReservedSectors;       //��������������������������
	BYTE NumFATs;               //FAT����
	WORD RootEntriesCount;      //����Ŀ¼��Ŀ����(FAT12��FAT16ʹ��)
	WORD TotalSectors16;         //������(FAT12��FAT16ʹ�ã�FAT32��ֵ��0��
	BYTE Media;                 //Media identifier
	WORD SectorsPerFAT16;       //ÿ���ĵ���������������FAT16ʹ�ã�
	WORD SecPerTrk;             //ÿ�ŵ�������
	WORD NumHeads;              //��ͷ��
	DWORD HiddenSectors;        //����ǰ��ʹ��������(MBR)
	DWORD TotalSectors32;       //�����������������65535���μ�ƫ��0x13��
	union {
		struct {
			BYTE _DrvNum;          //��������������(FAT16)
			BYTE _Reserved1;      //��ǰ��ͷ(FAT16)
			BYTE _BootSig;        //ǩ��(FAT16)
			DWORD _VolID;         //ID��FAT16��
			char _ValLab [ 11 ];   //Volume label
			char FilSysType [ 8 ]; //File system name"FAT12" "FAT16"
		}FAT1216;
		// FAT32
		struct {
			DWORD SectorsPerFAT32;       //Sectors per one FAT
			WORD ExtFlags;               //��չ��־λ0-3��� FAT��(��0��ʼ������������1)��λ4-6��������λ7��0ֵ��ζ��������ʱFAT��ӳ�䵽���е�FAT��1ֵ��ʾֻ��һ��FAT�ǻ�ģ�λ8-15������
			WORD FSVer;                  //�汾��
			DWORD RootClus;              //��Ŀ¼��ʼ��
			WORD FSInfo;                 //������Ϊ����ϵͳ�ṩ���ڿմ���������һ���ôص���Ϣ
			WORD BkBootSec;              //����������������λ���ļ�ϵͳ�� 6 �������� 
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
	BYTE Order;          //���ļ�����һ���ַ� �� ���ļ������ �� ɾ����־0xE5 �� δʹ��0��
	union{
		UINT64 ReferenceNumber;  //�������ʹ��		
		CHAR Name1[10];
	};
	BYTE Attr;  //0x01-ֻ��  0x02-����  0x04-ϵͳ�ļ�  0x08-��� 0x0F-Ϊ��ֵʱ��ʾ��Ŀ¼��Ϊ���ļ���Ŀ¼��  0x10-Ŀ¼ 0x20-�浵 
	BYTE Reserved1;
	union{
		struct{
// 			CHAR Name[8];
// 			CHAR ExtName[3];
			BYTE CreateTimeTength;
			WORD CreateTime;
			WORD CreateDate;
			WORD LastAccessDate;
			WORD ClusterNumberHigh;  //�ļ���ʼ�غŵĸ������ֽڡ� 
			WORD WriteTime;
			WORD WriteDate;
			WORD ClusterNumberLow; //�ļ�������ʼ�غŵĵ������ֽڣ�
			DWORD FileSize;
		};  //���ļ�����
		struct{
			BYTE Checksum;      //У��
			TCHAR Name2[6];
			WORD FirstCluster1;  //�̶�Ϊ0
			TCHAR Name3[2];
		}L; //���ļ�����Ե��������ڶ��ļ���ǰ��
	};
}*PFAT_FILE;

#pragma pack(pop)

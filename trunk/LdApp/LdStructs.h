#pragma once


//�������������������̽���ͨ�ŷ��ͺ���ID����ȷ����ݡ���ID�������Ӧ��
enum LD_FUNCTION_ID
{
	LFI_EARSE_FILE
};

//���ù��ܺ����ı�־��
enum LD_FUNCTION_FLAG
{
	LFF_NONE = 0x0,
	LFF_NEW_PROCESS = 0x00000001,
	LFF_NEW_WINDOW = 0x00000002,
	LFF_AS_ADMIN = 0x80000000,
};

#pragma pack(push, 1)
//���̼�ͨ�Ŵ��ݵ����ݸ�ʽ��
typedef struct COMMUINCATION_DATA
{
	union
	{
		LD_FUNCTION_ID fId;
		DWORD cId;
	};
	WORD nLength;
	BYTE Data[1];
}*PCOMMUINCATION_DATA;

#pragma pack(pop)


#pragma region �ļ�����
	//���ý��������в����еĵ�һ��������ָ�������Ǹ�������
#define CMD_ERASE    TEXT("/erase")      //�ļ�����

//����"�ļ�����"���������в�������
#define EPN_ERASE_FILES   TEXT("files")
#define ENP_ERASE_VOLUMES TEXT("volumes")
#define CMD_ANALY_FILES   TEXT("anafile")
#define CMD_ANALY_VOLUMES TEXT("anavolume")
#define EPN_MOTHED        TEXT("mothed")
#define EPN_NAME          TEXT("name")
#define EPN_UNDELFOLDER   TEXT("undelfolder")

//�ļ�����״̬
enum E_FILE_STATE
{
	efs_ready,      //׼��
	efs_erasuring,  //������
	efs_erasured,   //�Ѳ���
	efs_error,      //����ʧ�ܣ��д���
	efs_abort       //ȡ������
};
//�����̶߳���
enum E_THREAD_OPTION
{
	eto_start,      //�����߳̿�ʼ   
	eto_begin,      //��ʼ�����������ļ���
	eto_completed,  //������ɣ������ļ���
	eto_progress,   //�������ȣ������ļ���
	eto_freespace,  //���̿��пռ�
	eto_track,      //ɾ�����ļ��ۼ�
	eto_finished,   //ȫ���������
	eto_analy,      //���̷���
	eto_analied,    //���̷������
	eto_error,
	eto_abort
};

//�����̻߳ص�����
interface LDLIB_API IEraserListen
{
	//public:
	virtual bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) = 0;
};

/*
�ļ�������̬�⵼���ӿ�
����API_Init��ȡ����ӿڡ�

*/
interface LDLIB_API IErasureLibrary
{
	virtual DWORD EraseFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD EraseVolume(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaVolume(CDynObject& Param, IEraserListen * callback) = 0;
};

#pragma endregion  
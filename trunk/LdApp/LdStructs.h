#pragma once

#define HOST_PARAM_PLUGID _T("plug:")
//#define HOST_PARAM_PIPENAME _T("pn:")
//�������������������̽���ͨ�ŷ��ͺ���ID����ȷ����ݡ���ID�������Ӧ��
enum LD_FUNCTION_ID
{
	LFI_EARSE_FILE
};

//���ù��ܺ����ı�־��
enum LD_FUNCTION_FLAG
{
	LFF_NONE = 0x0, 
	LFF_NEW_PROCESS = 0x00000001,  //
	LFF_NEW_WINDOW = 0x00000002,   //
	LFF_AS_ADMIN = 0x80000000,     //Ҫ�����ԱȨ��
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

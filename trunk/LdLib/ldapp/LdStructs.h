#pragma once

namespace LeadowLib {
	//���ܺ���ID
	//

//#pragma warning(disable:4091) //������������

#ifdef _DEBUG
#ifdef WIN64
#define INVOKER_EXE _T("LdInvoker_d64.exe")
#else
#define INVOKER_EXE _T("LdInvoker_d32.exe")
#endif
#else
#ifdef WIN64
#else
#endif
#endif

//���ý��������в����еĵ�һ��������ָ�������Ǹ�������
#define CMD_ERASE_FILE    L"/erasefile"      //�ļ�����
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME  L"/erasevolume"

//����"�ļ�����"���������в�������
#define EPN_MOTHED        L"mothed"
#define EPN_FILE          L"file"
#define EPN_UNDELFOLDER   L"undelfolder"

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
			WORD cId;
		};
		WORD nLength;
		BYTE Data[1];
	}*PCOMMUINCATION_DATA;

#pragma pack(pop)

	/*
	ͨ�ûص�����
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

};
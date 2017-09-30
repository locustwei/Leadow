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

//���ý��̲����еĵ�һ��������ָ�������Ǹ�������
#define CMD_ERASE_FILE    L"/erasefile"      //
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME  L"/erasevolume"

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


	/*
	ͨ�ûص�����
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

};
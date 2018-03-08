#pragma once

#define HOST_PARAM_PLUGID _T("plug:")
//#define HOST_PARAM_PIPENAME _T("pn:")
//进程启动后与向主进程建立通信发送函数ID，以确认身份。（ID与参数对应）
enum LD_FUNCTION_ID
{
	LFI_EARSE_FILE
};

//调用功能函数的标志。
enum LD_FUNCTION_FLAG
{
	LFF_NONE = 0x0, 
	LFF_NEW_PROCESS = 0x00000001,  //
	LFF_NEW_WINDOW = 0x00000002,   //
	LFF_AS_ADMIN = 0x80000000,     //要求管理员权限
};

#pragma pack(push, 1)
//进程间通信传递的数据格式。
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

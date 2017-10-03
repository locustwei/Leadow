#pragma once

namespace LeadowLib {
	//功能函数ID
	//

//#pragma warning(disable:4091) //不理解这个警告

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

//调用进程命令行参数中的第一个参数，指明调用那个函数。
#define CMD_ERASE_FILE    L"/erasefile"      //文件擦除
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME  L"/erasevolume"

//调用"文件擦除"进程命令行参数名。
#define EPN_MOTHED        L"mothed"
#define EPN_FILE          L"file"
#define EPN_UNDELFOLDER   L"undelfolder"

//进程启动后与向主进程建立通信发送函数ID，以确认身份。（ID与参数对应）
	enum LD_FUNCTION_ID
	{
		LFI_EARSE_FILE
	};


	//调用功能函数的标志。
	enum LD_FUNCTION_FLAG
	{
		LFF_NONE = 0x0,
		LFF_NEW_PROCESS = 0x00000001,
		LFF_NEW_WINDOW = 0x00000002,
		LFF_AS_ADMIN = 0x80000000,
	};


#pragma pack(push, 1)
	//进程间通信传递的数据格式。
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
	通用回掉函数
	*/
	template <typename T>
	interface IGernalCallback
	{
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

};
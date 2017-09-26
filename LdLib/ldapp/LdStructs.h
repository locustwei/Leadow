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

#define CMD_ERASE_FILE    L"/erasefile"
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME  L"/erasevolume"

	enum LD_FUNCTION_ID
	{
		LFI_NONE,
		LFI_HIDE_FILE,
		LFI_DELETE_FILE,
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


	/*
	通用回掉函数
	*/
	template <typename T>
	class IGernalCallback
	{

	public:
		virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
	};

};
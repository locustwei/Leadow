#pragma once

#pragma warning(disable:4091)
#pragma warning(disable:4267)

#define LDLIB_API __declspec(dllexport)

typedef struct _LD_STRING
{
	UINT Length;
	PWCHAR Buffer;
}LD_STRING, *PLD_STRING;

//功能函数ID
//
typedef enum LD_FUNCTION_ID
{
	LFI_NONE,
	LFI_HIDE_FILE,
	LFI_DELETE_FILE
};
//调用功能函数的标志。
typedef enum LD_FUNCTION_FLAG
{
	LFF_NONE        = 0x0,
	LFF_NEW_PROCESS = 0x00000001,
	LFF_NEW_WINDOW  = 0x00000002,

	LFF_AS_ADMIN    = 0x80000000,
};
//通过命名管道进行参数传递。
//指示命名管道动作
typedef enum PIPE_FOLW_ACTION
{
	PFA_ERROR,
	PFA_CREATE,
	PFA_CONNECT,
	PFA_READ,
	PFA_WRITE,
	PFA_DONE
};
//命名管道数据提供接口。

typedef struct IPipeDataProvider
{
	virtual PIPE_FOLW_ACTION PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext) = 0;
};
//文件保护类型
typedef enum LD_FILE_PROTECT
{
	LFP_HIDE     = 0x0001,
	LFP_DELETE   = 0x0002,
	LFP_RENAME   = 0x0004,
	LFP_READ     = 0x0008,
	LFP_WRITE    = 0x0010,

	LFP_ALL      = LFP_HIDE | LFP_DELETE | LFP_RENAME | LFP_READ | LFP_WRITE
};

BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName);
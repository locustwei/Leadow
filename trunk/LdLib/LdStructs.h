#pragma once

#pragma warning(disable:4091)
#pragma warning(disable:4267)

//功能函数ID
//
typedef enum LD_FUNCTION_ID
{
	LFI_NONE,
	LFI_HIDE_FILE,
	LFI_DELETE_FILE,
	LFI_EARSE_FILE
};
//调用功能函数的标志。
typedef enum LD_FUNCTION_FLAG
{
	LFF_NONE        = 0x0,
	LFF_NEW_PROCESS = 0x00000001,
	LFF_NEW_WINDOW  = 0x00000002,

	LFF_AS_ADMIN    = 0x80000000,
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

/*
通用回掉函数
*/
template <typename T>
class IGernalCallback
{

public:
	virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
};

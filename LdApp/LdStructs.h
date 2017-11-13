#pragma once


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
		DWORD cId;
	};
	WORD nLength;
	BYTE Data[1];
}*PCOMMUINCATION_DATA;

#pragma pack(pop)


#pragma region 文件擦除
	//调用进程命令行参数中的第一个参数，指明调用那个函数。
#define CMD_ERASE    TEXT("/erase")      //文件擦除

//调用"文件擦除"进程命令行参数名。
#define EPN_ERASE_FILES   TEXT("files")
#define ENP_ERASE_VOLUMES TEXT("volumes")
#define CMD_ANALY_FILES   TEXT("anafile")
#define CMD_ANALY_VOLUMES TEXT("anavolume")
#define EPN_MOTHED        TEXT("mothed")
#define EPN_NAME          TEXT("name")
#define EPN_UNDELFOLDER   TEXT("undelfolder")

//文件擦除状态
enum E_FILE_STATE
{
	efs_ready,      //准备
	efs_erasuring,  //擦除中
	efs_erasured,   //已擦除
	efs_error,      //擦除失败（有错误）
	efs_abort       //取消操作
};
//擦除线程动作
enum E_THREAD_OPTION
{
	eto_start,      //控制线程开始   
	eto_begin,      //开始擦除（单个文件）
	eto_completed,  //擦除完成（单个文件）
	eto_progress,   //擦除进度（单个文件）
	eto_freespace,  //磁盘空闲空间
	eto_track,      //删除的文件痕迹
	eto_finished,   //全部擦除完成
	eto_analy,      //磁盘分析
	eto_analied,    //磁盘分析完成
	eto_error,
	eto_abort
};

//擦除线程回掉函数
interface LDLIB_API IEraserListen
{
	//public:
	virtual bool EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) = 0;
};

/*
文件擦除动态库导出接口
调用API_Init获取这个接口。

*/
interface LDLIB_API IErasureLibrary
{
	virtual DWORD EraseFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD EraseVolume(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaFile(CDynObject& Param, IEraserListen * callback) = 0;
	virtual DWORD AnaVolume(CDynObject& Param, IEraserListen * callback) = 0;
};

#pragma endregion  
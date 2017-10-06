#pragma once

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
	eto_analied     //磁盘分析完成
};

//擦除线程回掉函数
interface LDLIB_API IEraserThreadCallback
{
//public:
	virtual bool EraserThreadCallback(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue) = 0;
};

/*
文件擦除动态库导出接口
调用API_Init获取这个接口。

*/
interface LDLIB_API IErasureLibrary
{
//public:
	/*窗口资源ID
	*用于builer窗口*/
	//virtual TCHAR* UIResorce() = 0;
	/*
	窗口Build 完成后回传给Library(在动态库中Build窗口，貌似不太稳定)
	*/
	//virtual void SetUI(CControlUI*) = 0;
	//virtual CControlUI* GetUI() = 0;
	/*
	获取窗口消息处理对象（窗口Build好后，添加消息处理对象）
	*/
	//virtual CFramNotifyPump* GetNotifyPump() = 0;

	virtual DWORD EraseFile(CLdConfig& Param, IEraserThreadCallback * callback) = 0;
//protected:
	//virtual ~IErasureLibrary() {};
};

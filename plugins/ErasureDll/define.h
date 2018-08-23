#pragma once

#include "stdafx.h"


//函数调用参数名称
#define EPN_FILES                TEXT("files")
#define EPN_OP_REMOVEDIR         TEXT("rd")
#define EPN_OP_METHOD            TEXT("method")

#define EPN_ERROR_CODE            TEXT("error")


#define ENP_ERASE_VOLUMES TEXT("volumes")
#define EPN_MOTHED        TEXT("method")
#define EPN_UNDELFOLDER   TEXT("undelfolder")

enum ERASER_COMM_IDS
{
	eci_anafiles
};

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

typedef struct TEST_VOLUME_RESULT {
	UINT64 FileCount;        //文件数
	UINT64 DirectoryCount;   //目录数
	UINT64 FileTrackCount;   //MFT中删除文件的痕迹数
	UINT64 RecoverableCount; //删除文可以被恢复文件数（粗略数字及文件大小不为0）
	UINT   Writespeed;       //写文件速度（1G字节所用时间，单位毫秒）
	UINT   Cratespeed;       //创建0字节文件速度（100个文件所用时间，单位毫秒）
	UINT   Deletespeed;      //删除0字节文件的速度（100个文件所用时间，单位毫秒）
	DWORD ErrorCode;        //分析错误代码（如果）
}*PTEST_VOLUME_RESULT;

typedef struct TEST_FILE_RESULT {
	DWORD FileCount;        //文件数
	DWORD ADSCount;         //流个数
	UINT64 TotalSize;       //文件（目录）字节数
	UINT64 ADSSizie;        //流字节数
	DWORD ErrorCode;        //分析错误代码（如果）
}*PTEST_FILE_RESULT;

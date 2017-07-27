#pragma once
#include "Erasure.h"

enum E_FILE_TYPE
{
	eft_file,      //0: 文件；  
	eft_directory, //1：目录；
	eft_volume     //2：盘符
};

enum E_FILE_STATE
{
	efs_ready,      //准备
	efs_erasuring,  //擦除中
	efs_erasured,   //已擦除
	efs_error,      //擦除失败
	efs_abort       //取消操作
};
/*

//待擦除文件记录
typedef struct ERASURE_FILE_DATA
{
	E_FILE_TYPE  nFileType;           //文件类型
	E_FILE_STATE nStatus;             //擦除状态
	DWORD        nErrorCode;          //错误代码（如果错误）
	UINT_PTR     Tag;                 //
}*PERASURE_FILE_DATA;
*/

enum E_THREAD_OPTION
{
	eto_start,   //控制线程开始   
	eto_begin,    //开始擦除（单个文件）
	eto_completed, //擦除完成（单个文件）
	eto_progress,  //擦除进度（单个文件）
	eto_finished   //全部擦除完成
};
/*

//擦除过程中回掉函数参数
typedef struct ERASE_CALLBACK_PARAM
{
	E_THREAD_OPTION op; //当前操作
	float nPercent;     //进度%
	CFileInfo* pFile;  //文件记录
}*PERASE_CALLBACK_PARAM;
*/

class IEraserThreadCallback
{
public:
	virtual bool EraserThreadCallback(CFileInfo* pFile, E_THREAD_OPTION op, DWORD dwValue) = 0;
};
//文件擦除线程（同时创建多个文件擦除线程）
//线程池
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();     //终止擦除
	void SetEreaureFiles(CLdArray<CFileInfo*> * pFiles);  //添加擦除文件
	void SetEreaureMethod(CErasureMethod* pMethod);
	DWORD StartEreasure(UINT nMaxCount);            //开始擦除
protected:
	bool ReEresareFile(CLdArray<CFileInfo*>* files, int* pThredCount, bool& bWait, HANDLE* threads);
	void ControlThreadRun();                          //控制线程（同时最多创建m_nMaxThreadCount个擦除线程，结束一个再创建一个擦除线程）
	void ErasureThreadRun(CFileInfo* pData);  //单个文件擦除线程

	void ThreadRun(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;  //控制中途中断所有擦除线程
	boolean m_Abort;  //中断变量
	int m_nMaxThreadCount;  //最多线程数
	CThread* m_ControlThread; //控制线程
	CErasureMethod* m_Method;          //擦除算法

	IEraserThreadCallback* m_callback;  //擦除过程回掉函数，用于调用者界面操作
	CLdArray<CFileInfo*>* m_Files;    //待擦除的文件
	
	int WaitForThread(HANDLE* threads);

	//CEreaser 擦除操作回掉函数
	class CErasureCallbackImpl :      
		public IErasureCallback
	{
	public:
		CErasureCallbackImpl(CFileInfo* pFile);
		~CErasureCallbackImpl();

		CFileInfo* m_File;
		CEreaserThrads* m_Control;

		virtual BOOL ErasureStart(UINT nStepCount) override;
		virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
		virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;
	};
};
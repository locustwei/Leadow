#pragma once
#include "Erasure.h"

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
	eto_start,   //控制线程开始   
	eto_begin,    //开始擦除（单个文件）
	eto_completed, //擦除完成（单个文件）
	eto_progress,  //擦除进度（单个文件）
	eto_finished,   //全部擦除完成
	eto_analy,      //磁盘分析
	eto_analied     //磁盘分析完成
};

//擦除线程回掉函数
class IEraserThreadCallback
{
public:
	virtual bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, UINT dwValue) = 0;
};

//文件擦除线程（同时创建多个文件擦除线程）

class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();     //终止擦除
	void SetEreaureFiles(CLdArray<CVirtualFile*> * pFiles);  //添加擦除文件
	void SetEreaureMethod(CErasureMethod* pMethod);
	DWORD StartEreasure(UINT nMaxCount);            //开始擦除
	DWORD StartAnalysis(UINT nMaxCount);            //开始擦除
protected:

	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;  //控制中途中断所有擦除线程
	boolean m_Abort;  //中断变量
	int m_MaxThreadCount;  //最多线程数
	CThread* m_ControlThread; //控制线程
	CErasureMethod* m_Method;          //擦除算法

	IEraserThreadCallback* m_callback;  //擦除过程回掉函数，用于调用者界面操作
	CLdArray<CVirtualFile*>* m_Files;    //待擦除的文件
	LONG volatile m_ThreadCount;
	int WaitForThread();
	bool ReEresareFile(CLdArray<CVirtualFile*>* files/*, int* pThredCount, bool& bWait, HANDLE* threads*/);
	void ControlThreadRun(UINT_PTR Param);                          //控制线程（同时最多创建m_nMaxThreadCount个擦除线程，结束一个再创建一个擦除线程）
	void ErasureThreadRun(CVirtualFile* pData);  //单个文件擦除线程
	void AnalyThreadRung(CVolumeInfo* pVolume);    //磁盘分析线程 

	//CEreaser 擦除操作回掉函数
	class CErasureCallbackImpl :      
		public IErasureCallback
	{
	public:
		CErasureCallbackImpl(CVirtualFile* pFile);
		~CErasureCallbackImpl();

		CVirtualFile* m_File;
		CEreaserThrads* m_Control;

		virtual BOOL ErasureStart(UINT nStepCount) override;
		virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
		virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;
	};
};
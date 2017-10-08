/**************************************************
文件、磁盘擦除线程管理调度。
这个单元负责多线程同步问题处理，以及擦除对象与界面的
联系。
***************************************************/

#pragma once
#include "Erasure.h"
#include "../LdApp/LdStructs.h"

class CVolumeEx;

//擦除选项。
typedef struct ERASER_OPTIONS
{
	BOOL bRemoveFolder;      //删除空文件夹
	BOOL bSkipSpace;         //跳过空闲空间
	BOOL bSkipTrack;         //跳过删除痕迹
	BOOL bFreeFileSpace;     //擦除文件空闲
	ErasureMothedType FileMothed;    //文件擦除方法索引
	ErasureMothedType VolumeMothed;  //磁盘擦除方法索引
}*PERASER_OPTIONS;

//文件擦除线程（同时创建多个文件擦除线程）
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads();
	//CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();                                      //终止擦除
	void SetEreaureFiles(CLdArray<CVirtualFile*> * pFiles);  //添加擦除文件
	DWORD StartEreasure(UINT nMaxCount);            //开始擦除
	DWORD StartAnalysis(UINT nMaxCount);            //开始擦除
	PERASER_OPTIONS GetOptions();
	void SetCallback(IEraserThreadCallback* callback);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;                   //控制中途中断所有擦除线程
	boolean m_Abort;                   //停止擦除
	int m_MaxThreadCount;              //最多线程数
	CThread* m_ControlThread;          //控制线程
	ERASER_OPTIONS m_Options;          //
	CErasureMothed* m_FileMothed;
	CErasureMothed* m_VolumeMothed;

	IEraserThreadCallback* m_callback;  //擦除过程回掉函数，用于调用者界面操作
	CLdArray<CVirtualFile*>* m_Files;   //待擦除的文件
	LONG volatile m_ThreadCount;        //当前正在运行的线程数量

	int WaitForThread();                //当擦除线程达到最大线程数时等待其中一个线程结束
	bool ReEresareFile(CLdArray<CVirtualFile*>* files);   //遍历待擦除文件，一一创建擦除线程
	void ControlThreadRun(UINT_PTR Param);       //控制线程（同时最多创建m_nMaxThreadCount个擦除线程，结束一个再创建一个擦除线程）
	void ErasureThreadRun(CVirtualFile* pData);  //单个文件擦除线程
	void AnalyThreadRung(CVolumeEx* pVolume);    //磁盘分析线程 

	//CEreaser 擦除操作回掉函数
	class CErasureCallbackImpl :      
		public IErasureCallback
	{
	public:
		CErasureCallbackImpl(CVirtualFile* pFile);
		~CErasureCallbackImpl();

		CVirtualFile* m_File;
		CEreaserThrads* m_Control;

		virtual BOOL ErasureStart() override;
		virtual BOOL ErasureCompleted(DWORD dwErroCode) override;
		virtual BOOL ErasureProgress(ERASE_STEP nStep, UINT64 nMaxCount, UINT64 nCurent) override;
	};
};
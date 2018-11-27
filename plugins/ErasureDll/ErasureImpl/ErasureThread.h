
#pragma once

/**************************************************
文件、磁盘擦除线程管理调度。
这个单元负责多线程同步问题处理，以及擦除对象与界面的联系。
***************************************************/

#include "Erasure.h"
#include "../define.h"

typedef struct FILE_ERASURE_DATA
{
	E_FILE_STATE nStatus;             //擦除状态
	DWORD        nErrorCode;          //错误代码（如果错误）
	DWORD nCount;                     //文件夹下的总的文件数
	DWORD nErasued;                   //已经被擦除的文件数
}*PFILE_ERASURE_DATA;

//擦除线程回掉函数
interface IEraserListen
{
	//public:
	virtual bool EraserReprotStatus(TCHAR* FileName, TCHAR*, E_THREAD_OPTION op, LD_ERROR_CODE, INT_PTR Value) = 0;
	//virtual bool AnalyResult(E_THREAD_OPTION, TCHAR* FileName, PVOID pData) = 0;
};

//擦除选项。
typedef struct ERASER_OPTIONS
{
	BOOL bRemoveFolder;      //删除空文件夹
	BOOL bSkipSpace;         //跳过空闲空间
	BOOL bSkipTrack;         //跳过删除痕迹
	BOOL bFreeFileSpace;     //擦除文件空闲
	ErasureMothedType Mothed;    //文件擦除方法索引
}*PERASER_OPTIONS;

//文件擦除线程（同时创建多个文件擦除线程）
class CEreaserThrads 
	: public IThreadRunable
	, CThread
{
public:
	CEreaserThrads();
	~CEreaserThrads();

	void StopThreads();                                      //终止线程

	DWORD StartEraseFiles(CLdArray<TCHAR*>* Files, UINT nMaxCount);
	DWORD StartVolumeAnalysis(CLdArray<CLdString*>* Volumes, UINT nMaxCount);            //开始擦除

	PERASER_OPTIONS GetOptions();                    //获取、修改选项擦除选项
	void SetCallback(IEraserListen* callback);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;                   //控制中途中断所有擦除线程
	boolean m_Abort;                   //停止擦除
	int m_MaxThreadCount;              //最多线程数

	ERASER_OPTIONS m_Options;          //
	CErasureMothed* m_EraseMothed;

	IEraserListen* m_callback;         //擦除过程回掉函数，用于调用者界面操作
	CLdArray<CVirtualFile*> m_Files;   //待擦除的文件
	CLdArray<CLdString*> m_Volumes;    //待擦除的磁盘（GUID）
	LONG volatile m_ThreadCount;       //当前正在运行的线程数量

	int WaitForThread();               //当擦除线程达到最大线程数时等待其中一个线程结束
	DWORD InitThread(UINT nMaxCount);

	bool EresareFiles(CLdArray<CVirtualFile*>* files);   //遍历待擦除文件，一一创建擦除线程
	INT_PTR ErasureAFile(PVOID pData, UINT_PTR Param);   //单个文件擦除线程

	INT_PTR AanlysisAVolume(PVOID pData, UINT_PTR Param);
	bool AanlysisVolumes(CLdArray<CLdString *>* Volumes);

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
	private:
		CVirtualFile* m_Folder;
	};
};
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

//待擦除文件记录
typedef struct ERASURE_FILE_DATA
{
	TCHAR        cFileName[MAX_PATH]; //文件名
	E_FILE_TYPE  nFileType;  //文件类型
	E_FILE_STATE nStatus;    //擦除状态
	DWORD        nErrorCode; //错误代码（如果错误）
	CProgressUI* pProgress; //对应在界面的UI，擦除后删除之（不要在擦除线程里找UI同步很难处理）
}*PERASURE_FILE_DATA;


class CErasureThread: 
	public IThreadRunable,
	public IErasureCallback
{
public:
	CErasureThread(PERASURE_FILE_DATA pEraseFile);
	~CErasureThread();

	PERASURE_FILE_DATA m_Data;

	virtual BOOL ErasureStart(UINT nStepCount) override;
	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};

#define MAX_THREAD_COUNT 6  //同时开启文件擦除线程数
//擦除线程数组，
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads();
	void StopThreads();
	void AddEreaureFile(PERASURE_FILE_DATA pFile);
	DWORD StartEreasure(UINT nMaxCount);
protected:
	void ThreadRun(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	CRITICAL_SECTION cs;
	CLdArray<CErasureThread*> m_Threads;
	CLdArray<PERASURE_FILE_DATA> m_Files;
};
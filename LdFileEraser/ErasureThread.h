#pragma once
#include "Erasure.h"

enum E_FILE_TYPE
{
	eft_file,      //0: �ļ���  
	eft_directory, //1��Ŀ¼��
	eft_volume     //2���̷�
};

enum E_FILE_STATE
{
	efs_ready,      //׼��
	efs_erasuring,  //������
	efs_erasured,   //�Ѳ���
	efs_error,      //����ʧ��
	efs_abort       //ȡ������
};

//�������ļ���¼
typedef struct ERASURE_FILE_DATA
{
	TCHAR        cFileName[MAX_PATH]; //�ļ���
	E_FILE_TYPE  nFileType;  //�ļ�����
	E_FILE_STATE nStatus;    //����״̬
	DWORD        nErrorCode; //������루�������
	CProgressUI* pProgress; //��Ӧ�ڽ����UI��������ɾ��֮����Ҫ�ڲ����߳�����UIͬ�����Ѵ���
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

#define MAX_THREAD_COUNT 6  //ͬʱ�����ļ������߳���
//�����߳����飬
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
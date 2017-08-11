#pragma once
#include "Erasure.h"

//�ļ�����״̬
enum E_FILE_STATE
{
	efs_ready,      //׼��
	efs_erasuring,  //������
	efs_erasured,   //�Ѳ���
	efs_error,      //����ʧ�ܣ��д���
	efs_abort       //ȡ������
};
//�����̶߳���
enum E_THREAD_OPTION
{
	eto_start,   //�����߳̿�ʼ   
	eto_begin,    //��ʼ�����������ļ���
	eto_completed, //������ɣ������ļ���
	eto_progress,  //�������ȣ������ļ���
	eto_finished,   //ȫ���������
	eto_analy,      //���̷���
	eto_analied     //���̷������
};

//�����̻߳ص�����
class IEraserThreadCallback
{
public:
	virtual bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, UINT dwValue) = 0;
};

//�ļ������̣߳�ͬʱ��������ļ������̣߳�

class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();     //��ֹ����
	void SetEreaureFiles(CLdArray<CVirtualFile*> * pFiles);  //��Ӳ����ļ�
	void SetEreaureMethod(CErasureMethod* pMethod);
	DWORD StartEreasure(UINT nMaxCount);            //��ʼ����
	DWORD StartAnalysis(UINT nMaxCount);            //��ʼ����
protected:

	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;  //������;�ж����в����߳�
	boolean m_Abort;  //�жϱ���
	int m_MaxThreadCount;  //����߳���
	CThread* m_ControlThread; //�����߳�
	CErasureMethod* m_Method;          //�����㷨

	IEraserThreadCallback* m_callback;  //�������̻ص����������ڵ����߽������
	CLdArray<CVirtualFile*>* m_Files;    //���������ļ�
	LONG volatile m_ThreadCount;
	int WaitForThread();
	bool ReEresareFile(CLdArray<CVirtualFile*>* files/*, int* pThredCount, bool& bWait, HANDLE* threads*/);
	void ControlThreadRun(UINT_PTR Param);                          //�����̣߳�ͬʱ��ഴ��m_nMaxThreadCount�������̣߳�����һ���ٴ���һ�������̣߳�
	void ErasureThreadRun(CVirtualFile* pData);  //�����ļ������߳�
	void AnalyThreadRung(CVolumeInfo* pVolume);    //���̷����߳� 

	//CEreaser ���������ص�����
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
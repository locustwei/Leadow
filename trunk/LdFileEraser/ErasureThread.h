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
	E_FILE_TYPE  nFileType;           //�ļ�����
	E_FILE_STATE nStatus;             //����״̬
	DWORD        nErrorCode;          //������루�������
	UINT_PTR     Tag;                 //
}*PERASURE_FILE_DATA;

enum E_THREAD_OPTION
{
	eto_begin,     //�����߳̿�ʼ
	eto_start,     //��ʼ�����������ļ���
	eto_completed, //������ɣ������ļ���
	eto_progress,  //�������ȣ������ļ���
	eto_finished   //ȫ���������
};
//���������лص���������
typedef struct ERASE_CALLBACK_PARAM
{
	E_THREAD_OPTION op; //��ǰ����
	float nPercent;     //����%
	PERASURE_FILE_DATA pData;  //�ļ���¼
}*PERASE_CALLBACK_PARAM;


#define MAX_THREAD_COUNT 6  //ͬʱ�����ļ������߳���
//�ļ������̣߳�ͬʱ��������ļ������̣߳�
//�̳߳�
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads(IGernalCallback<PERASE_CALLBACK_PARAM>* callback);
	~CEreaserThrads();

	void StopThreads();     //��ֹ����
	void SetEreaureFiles(CTreeNodes<PERASURE_FILE_DATA> * pFiles);  //��Ӳ����ļ�
	void SetEreaureMethod(CErasureMethod* pMethod);
	DWORD StartEreasure(UINT nMaxCount);            //��ʼ����
protected:
	bool ReEresareFile(CTreeNodes<ERASURE_FILE_DATA*>* files, int* pThredCount, bool& bWait, HANDLE* threads);
	void ControlThreadRun();                          //�����̣߳�ͬʱ��ഴ��m_nMaxThreadCount�������̣߳�����һ���ٴ���һ�������̣߳�
	void ErasureThreadRun(PERASURE_FILE_DATA pData);  //�����ļ������߳�

	void ThreadRun(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;  //������;�ж����в����߳�
	boolean m_Abort;  //�жϱ���
	int m_nMaxThreadCount;  //����߳���
	CThread* m_ControlThread; //�����߳�
	CErasureMethod* m_Method;          //�����㷨

	IGernalCallback<PERASE_CALLBACK_PARAM>* m_callback;  //�������̻ص����������ڵ����߽������
	CTreeNodes<PERASURE_FILE_DATA> * m_Files;    //���������ļ�
	
	int WaitForThread(HANDLE* threads);

	//CEreaser ���������ص�����
	class CErasureCallbackImpl :      
		public IErasureCallback
	{
	public:
		CErasureCallbackImpl(PERASE_CALLBACK_PARAM pData);
		~CErasureCallbackImpl();

		PERASE_CALLBACK_PARAM m_Data;
		CEreaserThrads* m_Control;

		virtual BOOL ErasureStart(UINT nStepCount) override;
		virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
		virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;
	};
};
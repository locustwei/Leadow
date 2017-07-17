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
	CErasureMethod* pMethod;         //�����㷨
	TCHAR        cFileName[MAX_PATH]; //�ļ���
	E_FILE_TYPE  nFileType;  //�ļ�����
	E_FILE_STATE nStatus;    //����״̬
	DWORD        nErrorCode; //������루�������
	//CProgressUI* pProgress; //��Ӧ�ڽ����UI��������ɾ��֮����Ҫ�ڲ����߳�����UIͬ�����Ѵ���
}*PERASURE_FILE_DATA;

enum E_THREAD_OPTION
{
	eto_start,     //��ʼ����
	eto_completed, //������ɣ������ļ���
	eto_progress,  //��������
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
class CEreaserThrads : 
	public IThreadRunable,
	public ISortCompare<PERASURE_FILE_DATA>
{
public:
	CEreaserThrads(IGernalCallback<PERASE_CALLBACK_PARAM>* callback);
	~CEreaserThrads();

	void StopThreads();
	void AddEreaureFile(PERASURE_FILE_DATA pFile);  //��Ӳ����ļ�
	DWORD StartEreasure(UINT nMaxCount);            //��ʼ����
protected:
	void ControlThreadRun();                          //�����̣߳�ͬʱ��ഴ��m_nMaxThreadCount�������̣߳�����һ���ٴ���һ�������̣߳�
	void ErasureThreadRun(PERASURE_FILE_DATA pData);  //�����ļ������߳�

	void ThreadRun(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	int ArraySortCompare(ERASURE_FILE_DATA** it1, ERASURE_FILE_DATA** it2) override;
private:
	HANDLE m_hEvent;  //������;�ж����в����߳�
	boolean m_Abort;  //�жϱ���
	int m_nMaxThreadCount;  //����߳���

	IGernalCallback<PERASE_CALLBACK_PARAM>* m_callback;  //�������̻ص����������ڵ����߽������
	CLdArray<PERASURE_FILE_DATA> m_Files;    //���������ļ�

	class CErasureCallbackImpl :      //CEreaser ���������ص�����
		public IErasureCallback
	{
	public:
		CErasureCallbackImpl(PERASE_CALLBACK_PARAM pData);
		~CErasureCallbackImpl();

		PERASE_CALLBACK_PARAM m_Data;
		CEreaserThrads* threads;

		virtual BOOL ErasureStart(UINT nStepCount) override;
		virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override;
		virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override;
	};
};
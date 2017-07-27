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
/*

//�������ļ���¼
typedef struct ERASURE_FILE_DATA
{
	E_FILE_TYPE  nFileType;           //�ļ�����
	E_FILE_STATE nStatus;             //����״̬
	DWORD        nErrorCode;          //������루�������
	UINT_PTR     Tag;                 //
}*PERASURE_FILE_DATA;
*/

enum E_THREAD_OPTION
{
	eto_start,   //�����߳̿�ʼ   
	eto_begin,    //��ʼ�����������ļ���
	eto_completed, //������ɣ������ļ���
	eto_progress,  //�������ȣ������ļ���
	eto_finished   //ȫ���������
};
/*

//���������лص���������
typedef struct ERASE_CALLBACK_PARAM
{
	E_THREAD_OPTION op; //��ǰ����
	float nPercent;     //����%
	CFileInfo* pFile;  //�ļ���¼
}*PERASE_CALLBACK_PARAM;
*/

class IEraserThreadCallback
{
public:
	virtual bool EraserThreadCallback(CFileInfo* pFile, E_THREAD_OPTION op, DWORD dwValue) = 0;
};
//�ļ������̣߳�ͬʱ��������ļ������̣߳�
//�̳߳�
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();     //��ֹ����
	void SetEreaureFiles(CLdArray<CFileInfo*> * pFiles);  //��Ӳ����ļ�
	void SetEreaureMethod(CErasureMethod* pMethod);
	DWORD StartEreasure(UINT nMaxCount);            //��ʼ����
protected:
	bool ReEresareFile(CLdArray<CFileInfo*>* files, int* pThredCount, bool& bWait, HANDLE* threads);
	void ControlThreadRun();                          //�����̣߳�ͬʱ��ഴ��m_nMaxThreadCount�������̣߳�����һ���ٴ���һ�������̣߳�
	void ErasureThreadRun(CFileInfo* pData);  //�����ļ������߳�

	void ThreadRun(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;  //������;�ж����в����߳�
	boolean m_Abort;  //�жϱ���
	int m_nMaxThreadCount;  //����߳���
	CThread* m_ControlThread; //�����߳�
	CErasureMethod* m_Method;          //�����㷨

	IEraserThreadCallback* m_callback;  //�������̻ص����������ڵ����߽������
	CLdArray<CFileInfo*>* m_Files;    //���������ļ�
	
	int WaitForThread(HANDLE* threads);

	//CEreaser ���������ص�����
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
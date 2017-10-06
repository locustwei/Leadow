/**************************************************
�ļ������̲����̹߳������ȡ�
�����Ԫ������߳�ͬ�����⴦�����Լ���������������
��ϵ��
***************************************************/

#pragma once
#include "Erasure.h"
#include "../ErasureLibrary.h"

class CVolumeEx;

//����ѡ�
typedef struct ERASER_OPTIONS
{
	BOOL bRemoveFolder;      //ɾ�����ļ���
	BOOL bSkipSpace;         //�������пռ�
	BOOL bSkipTrack;         //����ɾ���ۼ�
	BOOL bFreeFileSpace;     //�����ļ�����
	ErasureMothedType FileMothed;    //�ļ�������������
	ErasureMothedType VolumeMothed;  //���̲�����������
}*PERASER_OPTIONS;

//�ļ������̣߳�ͬʱ��������ļ������̣߳�
class CEreaserThrads : 
	public IThreadRunable
{
public:
	CEreaserThrads();
	//CEreaserThrads(IEraserThreadCallback* callback);
	~CEreaserThrads();

	void StopThreads();                                      //��ֹ����
	void SetEreaureFiles(CLdArray<CLdString*> * pFiles);  //���Ӳ����ļ�
	DWORD StartEreasure(UINT nMaxCount);            //��ʼ����
	DWORD StartAnalysis(UINT nMaxCount);            //��ʼ����
	PERASER_OPTIONS GetOptions();
	void SetCallback(IEraserThreadCallback* callback);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;                   //������;�ж����в����߳�
	boolean m_Abort;                   //ֹͣ����
	int m_MaxThreadCount;              //����߳���
	CThread* m_ControlThread;          //�����߳�
	ERASER_OPTIONS m_Options;          //
	CErasureMothed* m_FileMothed;
	CErasureMothed* m_VolumeMothed;

	IEraserThreadCallback* m_callback;  //�������̻ص����������ڵ����߽������
	CLdArray<CLdString*>* m_Files;   //���������ļ�
	LONG volatile m_ThreadCount;        //��ǰ�������е��߳�����

	int WaitForThread();                //�������̴߳ﵽ����߳���ʱ�ȴ�����һ���߳̽���
	bool ReEresareFile(CLdArray<CVirtualFile*>* files);   //�����������ļ���һһ���������߳�
	void ControlThreadRun(UINT_PTR Param);       //�����̣߳�ͬʱ��ഴ��m_nMaxThreadCount�������̣߳�����һ���ٴ���һ�������̣߳�
	void ErasureThreadRun(CVirtualFile* pData);  //�����ļ������߳�
	void AnalyThreadRung(CVolumeEx* pVolume);    //���̷����߳� 

	//CEreaser ���������ص�����
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

#pragma once

/**************************************************
�ļ������̲����̹߳�����ȡ�
�����Ԫ������߳�ͬ�����⴦���Լ�����������������ϵ��
***************************************************/

#include "Erasure.h"
#include "../define.h"

typedef struct FILE_ERASURE_DATA
{
	E_FILE_STATE nStatus;             //����״̬
	DWORD        nErrorCode;          //������루�������
	DWORD nCount;                     //�ļ����µ��ܵ��ļ���
	DWORD nErasued;                   //�Ѿ����������ļ���
}*PFILE_ERASURE_DATA;

//�����̻߳ص�����
interface IEraserListen
{
	//public:
	virtual bool EraserReprotStatus(TCHAR* FileName, TCHAR*, E_THREAD_OPTION op, DWORD dwValue) = 0;
	virtual bool AnalyResult(TCHAR* FileName, PVOID pData) = 0;
};

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
class CEreaserThrads 
	: public IThreadRunable
	, CThread
{
public:
	CEreaserThrads();
	~CEreaserThrads();

	void StopThreads();                                      //��ֹ�߳�

	DWORD StartEraseFiles(CLdArray<TCHAR*>* Files, UINT nMaxCount);
	DWORD StartVolumeAnalysis(CLdArray<TCHAR*>* Volumes, UINT nMaxCount);            //��ʼ����

	PERASER_OPTIONS GetOptions();                    //��ȡ���޸�ѡ�����ѡ��
	void SetCallback(IEraserListen* callback);
protected:
	void ThreadBody(CThread* Sender, UINT_PTR Param) override;
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
private:
	HANDLE m_hEvent;                   //������;�ж����в����߳�
	boolean m_Abort;                   //ֹͣ����
	int m_MaxThreadCount;              //����߳���

	ERASER_OPTIONS m_Options;          //
	CErasureMothed* m_FileMothed;
	CErasureMothed* m_VolumeMothed;

	IEraserListen* m_callback;         //�������̻ص����������ڵ����߽������
	CLdArray<CVirtualFile*> m_Files;   //���������ļ�
	LONG volatile m_ThreadCount;       //��ǰ�������е��߳�����

	int WaitForThread();               //�������̴߳ﵽ����߳���ʱ�ȴ�����һ���߳̽���
	
	INT_PTR EraseFile_Thread();        //�ļ����������̣߳�Ϊÿ���ļ����������̣߳�������ͬʱ���е��߳�����
	bool EresareFiles(CLdArray<CVirtualFile*>* files);   //�����������ļ���һһ���������߳�
	INT_PTR ErasureAFile(CVirtualFile* pFile);           //�����ļ������߳�

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
	private:
		CVirtualFile* m_Folder;
	};
	DWORD InitThread(UINT nMaxCount);
};
#pragma once
#include "Thread.h"
#include "../LdDelegate.h"

namespace LeadowLib
{
	/*
	�ڴ�ӳ���ļ�ʵ���������̼��������ͨ�š�
	��һ���������̳�ΪMaster���ڶ�����ΪClient��

	*/

#pragma warning(disable: 4200)

	class CShareData: IThreadRunable
	{

	public:
		CShareData(TCHAR* pName, WORD nSize = 1024);
		~CShareData();
		CLdString& GetName();
		//д����
		virtual DWORD Write(PVOID pData, UINT nLength);
		//��ȡ���ݣ���û�ж�ȡ�����߳�ʱ���ã�
		//		virtual DWORD Read(PVOID pData, WORD nLength); 
		//virtual DWORD Read(PBYTE* pData, WORD* nLength);
		//�̵߳ȴ���ȡ����
		virtual DWORD StartReadThread(
			CMethodDelegate ReadCallback, //�������ݻص�
			bool FreeOnTerminate = false);        //ֹͣ��ȡʱɾ���Լ�
												  //ֹͣ��ȡ�����߳�
		void StopReadThread();

		void SetWaitTimeOut(UINT nTime);
		//		static CShareData* Create(TCHAR* pName, UINT nSize);

	private:
		typedef struct SHAREDATA_DATA
		{
			WORD id;       //�����˭д���
			WORD nSize;
			BYTE Data[0];
		}*PSHAREDATA_DATA;

		HANDLE m_hFile;            //�ڴ�ӳ���ļ�
		PSHAREDATA_DATA m_pFileHeader;   //ӳ���ļ��ڴ��ַ
		UINT m_Size;               //�ļ���С
		HANDLE m_hSemaphore;       //�ź�����ͬ����д�ڴ�ӳ���ļ�
		HANDLE m_hReadEvent;       //�ȴ�����д���¼���
		//HANDLE m_hWriteEvent;      //��������д�롣

		CLdString m_Name;          //�ڴ�ӳ���ļ���
		bool m_TermateReadThread;  //������ȡ�̡߳�
//		bool m_IsMaster;           //�Ƿ�ΪMaster
		UINT m_nTimeOut;           //WaitForд�롢��ȡ�ȴ���ʱ
		bool m_FreeOnTerminate;    //��ȡ�߳̽���ʱɾ���Լ�
		CMethodDelegate m_ReadCallback;
		WORD m_ThisID;            //ͬһ�����ļ���ʹ���߱��
	protected: //IThreadRunable
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	};

}

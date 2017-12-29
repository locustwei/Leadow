#pragma once
#include "Thread.h"

namespace LeadowLib
{
	/*
	�ڴ�ӳ���ļ�ʵ���������̼��������ͨ�š�
	��һ���������̳�ΪMaster���ڶ�����ΪClient��

	*/

#pragma warning(disable: 4200)

	class CShareData: IThreadRunable
	{
	private:
		struct MAP_DATA
		{
			BYTE nSign;     //1,��������Ѷ�ȡ
			WORD nSize;
			BYTE Data[0];
		};
		HANDLE m_hFile;            //�ڴ�ӳ���ļ�
		MAP_DATA* m_pFileHeader;   //ӳ���ļ��ڴ��ַ
		UINT m_Size;               //�ļ���С
		HANDLE m_hSemaphore;       //�ź�����ͬ����д�ڴ�ӳ���ļ�
		HANDLE m_hWaitEvent;       //�ȴ�����д���¼���
		HANDLE m_hSetEvent;        //��������д�롣

		CLdString m_Name;          //�ڴ�ӳ���ļ���
		bool m_TermateReadThread;  //������ȡ�̡߳�
		bool m_IsMaster;           //�Ƿ�ΪMaster
		UINT m_nTimeOut;
		CShareData(TCHAR* pName);
	protected:
		void ThreadBody(CThread* Sender, UINT_PTR Param) override;
		void OnThreadInit(CThread* Sender, UINT_PTR Param) override;
		void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;
	public:
		~CShareData();
		//д����
		DWORD Write(PVOID pData, UINT nLength);
		//��ȡ���ݣ���û�ж�ȡ�����߳�ʱ���ã�
		DWORD Read(PVOID pData, UINT nLength);  
		//�̵߳ȴ���ȡ����
		DWORD StartReadThread(IGernalCallback<PVOID>* ReadCallback);
		//ֹͣ��ȡ�����߳�
		void StopReadThread();

		void SetWaitTimeOut(UINT nTime);
		static CShareData* Create(TCHAR* pName, UINT nSize);
	};

}

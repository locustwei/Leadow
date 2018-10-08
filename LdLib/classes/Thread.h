#pragma once
#include "../LdDelegate.h"

namespace LeadowLib {

	class CThread;

	interface IThreadRunable 
	{
		//�߳����庯��
		virtual VOID ThreadBody(CThread* Sender, UINT_PTR Param) = 0;
		//OnThreadInit���̻߳�û�������������߳���
		virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) = 0;
		//�̼߳�������
		virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) = 0;
	};

	class CThread
	{
	public:
		CThread(void);
		CThread(IThreadRunable* pRumer);
		virtual ~CThread(void);

		int	GetPriority();
		BOOL SetPriority(int nPriority);
		DWORD Suspend();
		DWORD Resume();
		HANDLE GetHandle() const;
		DWORD GetId() const;  //�߳�Id
		BOOL  IsAlive() const;

		bool GetFreeOnTerminate() const;
		VOID SetFreeOnTerminate(bool value);
		bool GetTerminated() const;

		VOID SetTerminatee(bool value);

		/*
		��ʼ�߳�
		Param �ش����̻߳ص�����
		*/
		virtual HANDLE Start(UINT_PTR Param);
		virtual HANDLE Start(CLdMethodDelegate method, PVOID, UINT_PTR Param);
		/*
		�����߳�
		dwWaitTime �ȴ��߳̽���ʱ�䣬Ȼ��ǿ�ƽ���
		uExitCode �߳̽�������ֵ
		*/
		virtual DWORD Terminate(DWORD dwWaitTime = 0, DWORD uExitCode = 0);
		/*
		�滻ϵͳSleep��������ʹ��Wakeup����Sleep
		dwMilliseconds ˯��ʱ��
		*/
		DWORD Sleep(DWORD dwMilliseconds);
		VOID Wakeup(HANDLE hEvent) const;
		void SetTag(UINT_PTR nTag);
		UINT_PTR GetTag();
		IThreadRunable* GetRunner() { return m_Runer; };
	protected:
		virtual int	ThreadRun();
		virtual void ThreadInit();

		bool m_Terminated;  
		//���ڻ���Sleep���߳�Event
		HANDLE m_hSleep;
	private:
		HANDLE m_hThread;
		DWORD m_ThreadId;
		IThreadRunable* m_Runer;
		CLdMethodDelegate m_Delegate;
		bool m_FreeOnTerminate;
		UINT_PTR m_Param;
		UINT_PTR m_Tag;
		VOID ResetHandle();
		static DWORD WINAPI ThreadProcedure(LPVOID pParam);
		void DoTerminated();
		PVOID m_Data;
	};

};
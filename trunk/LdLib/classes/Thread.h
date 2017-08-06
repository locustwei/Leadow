#pragma once

namespace LeadowLib {

	class CThread;

	class IThreadRunable {
	public:
		virtual VOID ThreadRun(CThread* Sender, UINT_PTR Param) = 0;
		//�̻߳�ô�������������߳���
		virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) = 0;
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
		/*
		�����߳�
		dwWaitTime �ȴ��߳̽���ʱ�䣬
		uExitCode �߳̽�������ֵ
		*/
		virtual DWORD Terminate(DWORD dwWaitTime = 0, DWORD uExitCode = 0);
		/*
		�滻ϵͳSleep��������ʹ��Wakeup����Sleep
		dwMilliseconds ˯��ʱ��
		*/
		DWORD Sleep(DWORD dwMilliseconds);
		VOID Wakeup(HANDLE hEvent) const;
	protected:
		virtual int	ThreadRun();
		virtual void ThreadInit();
		bool m_Terminated;
		HANDLE m_hSleep;
	private:
		HANDLE m_hThread;
		DWORD m_ThreadId;
		IThreadRunable* m_Runer;
		bool m_FreeOnTerminate;
		WPARAM m_Param;
		VOID ResetHandle();
		static DWORD WINAPI ThreadProcedure(LPVOID pParam);
		void DoTerminated();
	};

};
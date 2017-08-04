#pragma once

namespace LeadowLib {
	//ͨ�������ܵ����в������ݡ�
	//ָʾ�����ܵ�����
	typedef enum PIPE_FOLW_ACTION
	{
		PFA_ERROR,
		PFA_CREATE,
		PFA_CONNECT,
		PFA_READ,
		PFA_WRITE,
		PFA_DONE
	};
	//�����ܵ������ṩ�ӿڡ�

	typedef struct IPipeDataProvider
	{
		virtual PIPE_FOLW_ACTION PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext) = 0;
	};

	class CLdNamedPipe
	{
	public:
		CLdNamedPipe();
		~CLdNamedPipe(void);

		BOOL Create(LPCTSTR lpPipeName);
		BOOL Open(LPCTSTR lpPipeName);
		UINT ReadData(LPVOID lpBuffer, UINT nSize);
		UINT WriteData(LPVOID lpBuffer, UINT nSize);
		BOOL WaitConnect();

		BOOL CreateFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback, PVOID pContext);
		BOOL OpenFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback, PVOID pContext);

		LPCTSTR GetPipeName();
	private:
		HANDLE m_hPipe;
		HANDLE m_hEvent;
		BOOL m_Connected;
		LPCTSTR m_PipeName;

	};

};
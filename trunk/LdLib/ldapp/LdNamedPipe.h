#pragma once

namespace LeadowLib {
	//通过命名管道进行参数传递。
	//指示命名管道动作
	typedef enum PIPE_FOLW_ACTION
	{
		PFA_ERROR,
		PFA_CREATE,
		PFA_CONNECT,
		PFA_READ,
		PFA_WRITE,
		PFA_DONE
	};
	//命名管道数据提供接口。

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
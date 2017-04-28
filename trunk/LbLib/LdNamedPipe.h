#pragma once

#define PIPENAME_PREFIX _T("\\\\.\\pipe\\")

typedef enum PIPE_FOLW_ACTION
{
	PFA_ERROR,
	PFA_CONNECTED,
	PFA_READ,
	PFA_WRITE,
	PFA_DONE
};

#pragma warning(disable:4091)

typedef struct IPipeDataProvider
{
	virtual PIPE_FOLW_ACTION PFACallback(UINT nStep, PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize) = 0;
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

	BOOL CreateFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback);
	BOOL OpenFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback);

private:
	HANDLE m_hPipe;
	BOOL m_Connected;
};




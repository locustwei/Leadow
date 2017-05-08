#pragma once
#include "LdStructs.h"

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
	BOOL m_Connected;
	LPCTSTR m_PipeName;
};




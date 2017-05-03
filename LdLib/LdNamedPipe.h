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

	BOOL CreateFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback);
	BOOL OpenFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback);

private:
	HANDLE m_hPipe;
	BOOL m_Connected;
};




#include "StdAfx.h"
#include "LdNamedPipe.h"
#include <stdio.h>

#define LDPIPE_WAIT_TIMEOUT 30000
#define PIPENAME_PREFIX _T("\\\\.\\pipe\\")

#pragma warning(disable:4996)

CLdNamedPipe::CLdNamedPipe()
{
	m_PipeName = NULL;
	m_hPipe = INVALID_HANDLE_VALUE;
	m_hEvent = INVALID_HANDLE_VALUE;
	m_Connected = false;
}


CLdNamedPipe::~CLdNamedPipe(void)
{
	if(m_Connected)
	{
		
		DWORD dwRead = 0, dwCb = 0;
		PeekNamedPipe(m_hPipe, &dwRead, sizeof(dwRead), &dwCb, NULL, NULL);
		if (dwCb > 0)
		
			WaitForSingleObject(m_hEvent, INFINITE);

		DisconnectNamedPipe(m_hPipe);
	}
	if(m_hPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hPipe);
	}
	if (m_hEvent != INVALID_HANDLE_VALUE)
		CloseHandle(m_hEvent);
}

BOOL CLdNamedPipe::Create(LPCTSTR lpPipeName)
{
	TCHAR* pTmpName = NULL;
	if (lpPipeName) {
		pTmpName = new TCHAR[11 + wcslen(lpPipeName)];
		swprintf(pTmpName, L"%s%s", PIPENAME_PREFIX, lpPipeName);
		m_PipeName = lpPipeName;
	}
	else
	{
		return FALSE;
	}

	m_hPipe = CreateNamedPipe(pTmpName, 
		PIPE_ACCESS_DUPLEX,
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES , 
		1024, 1024, LDPIPE_WAIT_TIMEOUT, NULL);
	
	if(pTmpName)
		delete [] pTmpName;

	m_hEvent = CreateEvent(NULL, FALSE, FALSE, lpPipeName);

	return m_hPipe != INVALID_HANDLE_VALUE;
}

UINT CLdNamedPipe::ReadData(LPVOID lpBuffer, UINT nSize)
{
	if(m_hPipe == INVALID_HANDLE_VALUE)
		return 0;
	
	DWORD dwCb = 0;
	DWORD dwWrite = 0;

	if (!ReadFile(m_hPipe, lpBuffer, nSize, &dwCb, NULL))
	{
		dwWrite = GetLastError();
		return 0;
	}
	SetEvent(m_hEvent);
	return dwCb;
}

UINT CLdNamedPipe::WriteData(LPVOID lpBuffer, UINT nSize)
{
	if(m_hPipe == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwCb = 0;
	DWORD dwRead = 0;
	WaitForSingleObject(m_hEvent, INFINITE);
	if (!WriteFile(m_hPipe, lpBuffer, nSize, &dwCb, NULL))
		return 0;
	return nSize;
}

BOOL CLdNamedPipe::CreateFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback, PVOID pContext)
{
	BOOL b;
	PIPE_FOLW_ACTION  next;
	LPVOID lpBuffer = NULL;
	UINT nSize = 0;

	if(callback == NULL)
		return false;

	b = Create(lpPipeName);
	next = PFA_CREATE;

	while(b)
	{
		next = callback->PFACallback(next, lpBuffer, nSize, pContext);
		if(next == PFA_DONE || next == PFA_ERROR)
			break;

		switch(next)
		{
		case PFA_CONNECT:
			b = WaitConnect();
			break;
		case PFA_READ:
			nSize = ReadData(lpBuffer, nSize);
			b = nSize > 0;
			break;
		case PFA_WRITE:
			nSize = WriteData(lpBuffer, nSize);
			b = nSize > 0;
			break;
		}

	}

	return b;
}

BOOL CLdNamedPipe::Open(LPCTSTR lpPipeName)
{
	TCHAR* pTmpName = new TCHAR[11 + wcslen(lpPipeName)];
	swprintf(pTmpName, L"%s%s", PIPENAME_PREFIX, lpPipeName);

	if(!WaitNamedPipe(pTmpName, LDPIPE_WAIT_TIMEOUT))
	{
		return false;
	}

	m_hPipe = CreateFile(pTmpName,  GENERIC_READ | GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	delete [] pTmpName;
	if (m_hPipe == INVALID_HANDLE_VALUE)
		return FALSE;

	m_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, lpPipeName);
	if (m_hPipe == INVALID_HANDLE_VALUE)
		return FALSE;
	
	SetEvent(m_hEvent);

	return TRUE;
}

BOOL CLdNamedPipe::OpenFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback, PVOID pContext)
{
	BOOL b;
	PIPE_FOLW_ACTION next;
	LPVOID lpBuffer = NULL;
	UINT nSize = 0;

	if(callback == NULL)
		return false;

	b = Open(lpPipeName);
	next = PFA_CONNECT;

	while(b)
	{
		next = callback->PFACallback(next, lpBuffer, nSize, pContext);
		if(next == PFA_DONE || next == PFA_ERROR)
			break;

		switch(next)
		{
		case PFA_READ:
			nSize = ReadData(lpBuffer, nSize);
			break;
		case PFA_WRITE:
			nSize = WriteData(lpBuffer, nSize);
			break;
		}

		b = nSize > 0;
	}

	return b;
}

LPCTSTR CLdNamedPipe::GetPipeName()
{
	return m_PipeName;
}

BOOL CLdNamedPipe::WaitConnect()
{
	m_Connected = ConnectNamedPipe(m_hPipe, NULL);

	return m_Connected;
}

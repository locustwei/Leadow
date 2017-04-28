#include "StdAfx.h"
#include "LdNamedPipe.h"
#include <stdio.h>

#define LDPIPE_WAIT_TIMEOUT 30000

#pragma warning(disable:4996)

CLdNamedPipe::CLdNamedPipe()
{
	m_hPipe = INVALID_HANDLE_VALUE;
	m_Connected = false;
}


CLdNamedPipe::~CLdNamedPipe(void)
{
	if(m_Connected)
	{
		DisconnectNamedPipe(m_hPipe);
	}
	if(m_hPipe != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hPipe);
	}
}

BOOL CLdNamedPipe::Create(LPCTSTR lpPipeName)
{
	TCHAR* pTmpName = NULL;
	if(lpPipeName == NULL)
	{
		return false;
		/*
		pTmpName = new TCHAR[11];
		srand( (unsigned)time( NULL ) );

		for(int i=0; i<10; i++)
			pTmpName[i] = 'A' + (1000 * rand()) % 25;
		pTmpName[10] = 0;
		lpPipeName = pTmpName;
		*/
	}
	pTmpName = new TCHAR[11 + wcslen(lpPipeName)];
	swprintf(pTmpName, L"%s%s", PIPENAME_PREFIX, lpPipeName);

	m_hPipe = CreateNamedPipe(pTmpName, 
		PIPE_ACCESS_DUPLEX | FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED, 
		PIPE_WAIT, 
		PIPE_UNLIMITED_INSTANCES , 
		1024, 1024, LDPIPE_WAIT_TIMEOUT, NULL);
	
	if(pTmpName)
		delete [] pTmpName;

	if(m_hPipe == INVALID_HANDLE_VALUE)
		return false;
	

	m_Connected = ConnectNamedPipe(m_hPipe, NULL);

	if(!m_Connected)
	{
		if(m_hPipe != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hPipe);
			m_hPipe = INVALID_HANDLE_VALUE;
		}
	}
	return m_Connected;
}

UINT CLdNamedPipe::ReadData(LPVOID lpBuffer, UINT nSize)
{
	if(m_hPipe == INVALID_HANDLE_VALUE)
		return 0;
	
	DWORD dwCb = 0;

	if(!ReadFile(m_hPipe, lpBuffer, nSize, &dwCb, NULL))
		return 0;
	return dwCb;
}

UINT CLdNamedPipe::WriteData(LPVOID lpBuffer, UINT nSize)
{
	if(m_hPipe == INVALID_HANDLE_VALUE)
		return 0;

	DWORD dwCb = 0;

	if(!WriteFile(m_hPipe, lpBuffer, nSize, &dwCb, NULL))
		return 0;
	return dwCb;
}

BOOL CLdNamedPipe::CreateFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback)
{
	BOOL b;
	PIPE_FOLW_ACTION  next;
	LPVOID lpBuffer = NULL;
	UINT nSize = 0, nStep = 0;

	if(callback == NULL)
		return false;

	b = Create(lpPipeName);
	next = PFA_CONNECTED;

	while(b)
	{
		next = callback->PFACallback(nStep++, next, lpBuffer, nSize);
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

BOOL CLdNamedPipe::Open(LPCTSTR lpPipeName)
{
	if(!WaitNamedPipe(lpPipeName, LDPIPE_WAIT_TIMEOUT))
	{
		return false;
	}

	TCHAR* pTmpName = new TCHAR[11 + wcslen(lpPipeName)];
	swprintf(pTmpName, L"%s%s", PIPENAME_PREFIX, lpPipeName);

	m_hPipe = CreateFile(pTmpName,  GENERIC_READ | GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	delete [] pTmpName;
	return m_hPipe == INVALID_HANDLE_VALUE;
}

BOOL CLdNamedPipe::OpenFlow(LPCTSTR lpPipeName, IPipeDataProvider* callback)
{
	BOOL b;
	PIPE_FOLW_ACTION next;
	LPVOID lpBuffer = NULL;
	UINT nSize = 0, nStep = 0;

	if(callback == NULL)
		return false;

	b = Open(lpPipeName);
	next = PFA_CONNECTED;

	while(b)
	{
		next = callback->PFACallback(nStep++, next, lpBuffer, nSize);
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

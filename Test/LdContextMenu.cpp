// LdContextMenu.cpp : CLdContextMenu µÄÊµÏÖ

#include "stdafx.h"
#include "LdContextMenu.h"
#include <string.h>
#include "LdNamedPipe.h"

using namespace std;

HRESULT CLdContextMenu::InvokeCommand(LD_FUNCTION_ID id)
{
	m_FunctionId = id;

	TCHAR PipeName[30] = { 0 };
	CLdNamedPipe Pipe;
	wsprintf(PipeName, _T("LdPipe%x"), (UINT)this);

	Pipe.CreateFlow(PipeName, this, PipeName);

	return S_OK;
}
BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName)
{
	TCHAR FileName[MAX_PATH] = { 0 };
	if (!GetModuleFileName(0, FileName, MAX_PATH))
		return false;

	TCHAR* s = _tcsrchr(FileName, '\\');
	if (s == NULL)
		return false;
	s += 1;
	_tcscpy(s, _T("LdInvoker_d64.exe"));

	TCHAR Params[100] = { 0 };
	wsprintf(Params, _T("%d %d %s"), id, Flag, lpPipeName);
	return ShellExecute(NULL, NULL, FileName, Params, NULL, SW_SHOWNORMAL) != NULL;

}

PIPE_FOLW_ACTION CLdContextMenu::PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext)
{
	PIPE_FOLW_ACTION Result = PFA_ERROR;
	UINT Length = 0;

	switch(current)
	{
	case PFA_CREATE:
		Result = PFA_CONNECT;
		RunInvoker(m_FunctionId, 0, (LPTSTR)pContext);
		break;
	case PFA_CONNECT:
		Result = PFA_WRITE;
		for(int i=0; i<m_SelectCount; i++)
		{
			Length += (wcslen(m_SelectFiles[i])+1) * sizeof(TCHAR);
		}
		lpBuffer = malloc(Length);
		ZeroMemory(lpBuffer, Length);
		nBufferSize = Length;
		Length = 0;
		for(int i=0; i<m_SelectCount; i++)
		{
			MoveMemory((PCHAR)lpBuffer+Length, m_SelectFiles[i], wcslen(m_SelectFiles[i]) * sizeof(TCHAR));
			Length += (wcslen(m_SelectFiles[i])+1) * sizeof(TCHAR); 
		}
		break;
	default:
		Result = PFA_DONE;
		break;
	}
	return Result;
}

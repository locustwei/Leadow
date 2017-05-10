// LdContextMenu.cpp : CLdContextMenu µÄÊµÏÖ

#include "stdafx.h"
#include "FileProtectPipeFlow.h"
#include "LdNamedPipe.h"


CFileProtectPipeFlow::CFileProtectPipeFlow()
{
	ZeroMemory(m_PipeName, ARRAYSIZE(m_PipeName));
	wsprintf(m_PipeName, _T("LdPipe%x"), (UINT)this);
}

CFileProtectPipeFlow::~CFileProtectPipeFlow()
{

}

BOOL CFileProtectPipeFlow::StartPipeFlow(int nFileCount, LPTSTR* lpFiles)
{
	FLOW_CONTEXT Context = { 0 };

	CLdNamedPipe Pipe;
	CFileProtectPipeFlow Flow;
	Flow.nCount = nFileCount;
	Flow.lpFiles = lpFiles;
	Pipe.CreateFlow(Flow.m_PipeName, &Flow, &Context);

	return S_OK;
}

PIPE_FOLW_ACTION CFileProtectPipeFlow::PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext)
{
	PIPE_FOLW_ACTION Result = PFA_ERROR;
	UINT Length = 0;
	FLOW_CONTEXT* p = (FLOW_CONTEXT*)pContext;

	switch(current)
	{
	case PFA_CREATE:
		Result = PFA_CONNECT;
		RunInvoker(LFI_HIDE_FILE, 0, m_PipeName);
		break;
	case PFA_CONNECT:
		Result = PFA_WRITE;
		lpBuffer = &nCount;
		nBufferSize = sizeof(nCount);
		break;
	case PFA_WRITE:
		if (p->nStep >= nCount)
		{
			Result = PFA_DONE;
			break;
		}
		lpBuffer = lpFiles[p->nStep];
		nBufferSize = wcslen(lpFiles[p->nStep]) * sizeof(TCHAR);
		p->nStep++;
		Result = PFA_WRITE;
		break;
	}
	return Result;
}

// LdContextMenu.cpp : CLdContextMenu µÄÊµÏÖ

#include "stdafx.h"
#include "FilePipeFlow.h"
#include "LdNamedPipe.h"


CFilePipeFlow::CFilePipeFlow()
{
	ZeroMemory(m_PipeName, ARRAYSIZE(m_PipeName));
	wsprintf(m_PipeName, _T("LdPipe%x"), (UINT)this);
}

CFilePipeFlow::~CFilePipeFlow()
{

}

BOOL CFilePipeFlow::StartProtectFlow(int nFileCount, LPTSTR* lpFiles)
{
	FLOW_CONTEXT Context = { 0 };
	
	CLdNamedPipe Pipe;
	CFilePipeFlow Flow;
	Flow.m_FId = LFI_HIDE_FILE;
	Flow.nCount = nFileCount;
	Flow.lpFiles = lpFiles;
	return Pipe.CreateFlow(Flow.m_PipeName, &Flow, &Context);
}

BOOL CFilePipeFlow::StartDeleteFlow(int nFileCount, LPTSTR * lpFiles)
{
	FLOW_CONTEXT Context = { 0 };

	CLdNamedPipe Pipe;
	CFilePipeFlow Flow;
	Flow.m_FId = LFI_DELETE_FILE;
	Flow.nCount = nFileCount;
	Flow.lpFiles = lpFiles;
	return Pipe.CreateFlow(Flow.m_PipeName, &Flow, &Context);
}

PIPE_FOLW_ACTION CFilePipeFlow::PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext)
{
	PIPE_FOLW_ACTION Result = PFA_ERROR;
	UINT Length = 0;
	FLOW_CONTEXT* p = (FLOW_CONTEXT*)pContext;

	switch(current)
	{
	case PFA_CREATE:
		Result = PFA_CONNECT;
		RunInvoker(m_FId, 0, m_PipeName);
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

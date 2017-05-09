// LdContextMenu.cpp : CLdContextMenu µÄÊµÏÖ

#include "stdafx.h"
#include "FileProtectPipeFlow.h"
#include <string.h>
#include "LdNamedPipe.h"

using namespace std;

CFileProtectPipeFlow::CFileProtectPipeFlow()
{
	ZeroMemory(m_PipeName, ARRAYSIZE(m_PipeName));
	wsprintf(m_PipeName, _T("LdPipe%x"), (UINT)this);
	nCount = 3;
	lpFiles = new LPTSTR[nCount];
	lpFiles[0] = _T("0aasdfasdfasd");
	lpFiles[1] = _T("1aasdfasdfasd");
	lpFiles[2] = _T("2aasdfasdfasd");
}

CFileProtectPipeFlow::~CFileProtectPipeFlow()
{

}

BOOL CFileProtectPipeFlow::StartPipeFlow()
{
	FLOW_CONTEXT Context = { 0 };

	CLdNamedPipe Pipe;
	Pipe.CreateFlow(m_PipeName, this, &Context);

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

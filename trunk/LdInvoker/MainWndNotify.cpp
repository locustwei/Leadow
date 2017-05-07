#include "stdafx.h"
#include <Shlwapi.h>
#include "MainWndNotify.h"
#include "LdInvoker.h"
#include "LdStructs.h"

LD_FUNCTION_ID functionId = LFI_NONE;
LD_FUNCTION_FLAG functionFlag = LFF_NONE;

CMainWndNotify::CMainWndNotify()
{
}


CMainWndNotify::~CMainWndNotify()
{
}

LRESULT CMainWndNotify::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	__super::OnDestroy(uMsg, wParam, lParam, bHandled);
	::PostQuitMessage(0L);
	return 0;
}

LPCTSTR CMainWndNotify::GetWndClassName()
{
	return szWindowClass;
}

LRESULT CMainWndNotify::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT ret = __super::OnCreate(uMsg, wParam, lParam, bHandled);

	functionId = (LD_FUNCTION_ID)StrToInt(ParamStrs[0]);
	functionFlag = (LD_FUNCTION_FLAG)StrToInt(ParamStrs[1]);

	//InvokeLdFunc(functionId, (LD_FUNCTION_FLAG)(functionFlag & ~LFF_NEW_PROCESS), NULL);

	return ret;
}

#include "stdafx.h"
#include <Shlwapi.h>
#include "MainWndNotify.h"
#include "LdInvoker.h"
#include "LdStructs.h"
#include "LdFunction.h"
/*
LD_FUNCTION_ID functionId = LFI_NONE;
LD_FUNCTION_FLAG functionFlag = LFF_NONE;
*/
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

void CMainWndNotify::Init()
{
	//functionId = (LD_FUNCTION_ID)StrToInt(ParamStrs[0]);
	//functionFlag = (LD_FUNCTION_FLAG)StrToInt(ParamStrs[1]);

	//InvokeLdFunc(functionId, ParamStrs[2], GetWnd()->GetHWND());

	//GetWnd()->Close();
	//::PostQuitMessage(0L);
}


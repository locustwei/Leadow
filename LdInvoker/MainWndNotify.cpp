#include "stdafx.h"
#include "MainWndNotify.h"


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
	return __super::GetWndClassName();
}

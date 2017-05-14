#include "stdafx.h"
#include "ProcessListView.h"


CProcessListView::CProcessListView()
{
}


CProcessListView::~CProcessListView()
{
}

DWORD CProcessListView::SelectFlags(HWND hParentWnd)
{
	DWORD ret = 0;

	CProcessListView Notify;
	Notify.SetSkinXml(_T("processlistview.xml"));

	CLdDuiWnd* pFrame = new CLdDuiWnd(&Notify);
	if (pFrame)
	{
		pFrame->Create(hParentWnd, _T(""), UI_WNDSTYLE_DIALOG, 0L);
		pFrame->CenterWindow();

		if (pFrame->ShowModal() == IDOK)
			ret = 0;

	}

	return ret;
}

void CProcessListView::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK) {
	}

	return __super::Notify(msg);
}

LRESULT CProcessListView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return __super::OnCreate(uMsg, wParam, lParam, bHandled);
}

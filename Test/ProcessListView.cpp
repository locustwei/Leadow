#include "stdafx.h"
#include "ProcessListView.h"


CProcessListView::CProcessListView(TCHAR* xmlSkin):CLdDuiWnd(xmlSkin)

{
}


CProcessListView::~CProcessListView()
{
}

DWORD CProcessListView::SelectFlags(HWND hParentWnd)
{
	DWORD ret = 0;

	CProcessListView*  pFrame = new CProcessListView(_T("processlistview.xml"));

	if (pFrame)
	{
		pFrame->Create(hParentWnd, _T(""), UI_WNDSTYLE_DIALOG, 0L);
		pFrame->CenterWindow();

		if (pFrame->ShowModal() == IDOK)
			ret = 0;
		delete pFrame;
	}

	return ret;
}

DuiLib::CListHeaderUI* CProcessListView::AddHeader()
{
	if (!m_List)
		return NULL;
	m_List->GetHeader()->Add()
}

void CProcessListView::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK) {
	}

	return __super::Notify(msg);
}

LRESULT CProcessListView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LRESULT ret = __super::OnCreate(uMsg, wParam, lParam, bHandled);
	m_btnOk = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnOk")));
	m_btnCancel = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnCancel")));

	m_List = static_cast<CListUI*>(GetPaintManager()->FindControl(_T("listview")));
	return ret;
}

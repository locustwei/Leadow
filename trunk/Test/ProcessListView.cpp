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
		pFrame->AddHeader(L"ddddddddddd", 150, -1);
		if (pFrame->ShowModal() == IDOK)
			ret = 0;

		delete pFrame;
	}

	return ret;
}

BOOL CProcessListView::AddHeader(CListHeaderItemUI * pHeaderItem, int nCol)
{
	if (!m_List)
		return FALSE;
	CListHeaderUI* pHeader = m_List->GetHeader();
	if (!pHeader)
		return FALSE;
	if (nCol == -1)
		nCol = pHeader->GetCount();

	return pHeader->AddAt(pHeaderItem, nCol);
}

CListHeaderItemUI * CProcessListView::AddHeader(LPCTSTR lpText, UINT nFixedWidth, int nCol)
{
	CListHeaderItemUI* pItemUI = new CListHeaderItemUI();
	pItemUI->SetText(lpText);
	pItemUI->SetTextStyle(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pItemUI->SetFixedWidth(nFixedWidth);
	pItemUI->SetAttribute(_T("hotimage"), _T("listimg/list_header_hot.png"));
	pItemUI->SetAttribute(_T("pushedimage"), _T("listimg/list_header_pushed.png"));
	pItemUI->SetAttribute(_T("sepimage"), _T("listimg/list_header_sep.png"));
	pItemUI->SetAttribute(_T("sepwidth"), _T("1"));
	if (!AddHeader(pItemUI, nCol))
	{
		delete pItemUI;
		return NULL;
	}
	else
		return pItemUI;
}

void CProcessListView::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)  //dbclick;
	{

	}
	else if (msg.sType == DUI_MSGTYPE_MENU)
	{
		GetPopupMenu(L"listContext")->Popup(this);
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

void CProcessListView::OnMenuItemClick(CLdMenu * pMenu, int id)
{
	MessageBox(GetHWND(), L"dddddddddddd", pMenu->GetName(), 0);
}

LPCTSTR CProcessListView::GetItemText(CControlUI * pList, int iItem, int iSubItem)
{
	return LPCTSTR();
}

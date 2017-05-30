#include "stdafx.h"
#include "ProcessListView.h"
#include "LdLib.h"



CProcessListView::CProcessListView(TCHAR* xmlSkin):CLdDuiWnd(xmlSkin)
{
}


CProcessListView::~CProcessListView()
{
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

int CProcessListView::AddItem(CListElementUI* pListItem, int nIndex)
{
	if (nIndex = -1)
		m_List->Add(pListItem);
	else
		m_List->AddAt(pListItem, nIndex);
	return pListItem->GetIndex();
}

CListTextElementUI * CProcessListView::AddTextItem()
{
	CListTextElementUI* pListElement = new CListTextElementUI();
	AddItem(pListElement);
	return pListElement;
}

BOOL CProcessListView::KillProcess(HWND hParentWnd, PDWORD pids, UINT nCount)
{
	BOOL ret = FALSE;

	CProcessListView*  pFrame = new CProcessListView(_T("processlistview.xml"));

	if (pFrame)
	{
		pFrame->Create(hParentWnd, _T(""), UI_WNDSTYLE_DIALOG, 0L);
		pFrame->CenterWindow();
		for (UINT i = 0; i < nCount; i++)
		{
			pFrame->AddTextItem()->SetTag(pids[i]);
		}
		if (pFrame->ShowModal() == IDOK)
			ret = TRUE;

		delete pFrame;
	}

	return ret;
}

void CProcessListView::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK) {
		if (msg.pSender == m_btnOk) {
			PostMessage(WM_CLOSE, IDOK, 0);
			return;
		}
		else if (msg.pSender == m_btnCancel) {
			Close();
			return;
		}
	}
	else if (msg.sType == DUI_MSGTYPE_MENU)
	{
		CControlUI* pControl = GetPaintManager()->FindControl(msg.ptMouse); 
		if (pControl && _tcsicmp(pControl->GetClass(), DUI_CTR_LISTTEXTELEMENT) == 0)
		{
			TCHAR fileName[MAX_PATH] = { 0 };
			CDuiString s(_T("/e, /select, "));
			UINT cid = GetPopupMenu(L"listContext")->Popup(this);
			switch (cid)
			{
			case 1:
				if(CProcessUtils::KillProcess(pControl->GetTag())==0)
					m_List->Remove(pControl);

				break;
			case 2:
				CProcessUtils::GetProcessFileName(pControl->GetTag(), fileName);
				s += fileName;
				OpenURL(_T("explorer"),  s);
				break;
			default:
				break;
			}
		}
	}

	return __super::Notify(msg);
}

LRESULT CProcessListView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LRESULT ret = __super::OnCreate(uMsg, wParam, lParam, bHandled);
	m_btnOk = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnOk")));
	m_btnCancel = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnCancel")));

	m_List = static_cast<CListUI*>(GetPaintManager()->FindControl(_T("listview")));
	m_List->SetTextCallback(this);
	return ret;
}

void CProcessListView::OnMenuItemClick(CLdMenu * pMenu, int id)
{
	
	//MessageBox(GetHWND(), L"dddddddddddd", pMenu->GetName(), 0);
}

LPCTSTR CProcessListView::GetItemText(CControlUI * pListItem, int iItem, int iSubItem)
{
	DWORD pid = pListItem->GetTag();
	TCHAR str[MAX_PATH] = { 0 };
	switch (iSubItem)
	{
	case 0:
		wsprintf(str, L"%d", pid);
		break;
	case 1:
		CProcessUtils::GetProcessFileName(pid, str);
		break;
	default:
		break;
	}
	return str;
}

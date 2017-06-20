#include "stdafx.h"
#include "Library.h"
#include "MainWnd.h"


DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(TCHAR* xmlSkin)
{
	m_Skin = xmlSkin;
	m_ErasureLib = NULL;
	m_ProtectLib = NULL;
}

CMainWnd::~CMainWnd()
{
	if (m_ErasureLib)
		delete m_ErasureLib;
	if (m_ProtectLib)
		delete m_ProtectLib;
}


DuiLib::CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode /*= NULL*/)
{
	return NULL;
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	return __super::Notify(msg);
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("LdFileManagerMainWnd");
}

LRESULT CMainWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = __super::HandleMessage(uMsg, wParam, lParam);
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return Result;
}

DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return m_Skin;
}

CDuiString CMainWnd::GetSkinFolder()
{
	return _T("skin");
}

void CMainWnd::OnClick(TNotifyUI& msg)
{
	return __super::OnClick(msg);
}

void CMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	if (msg.sType == _T("selectchanged"))
	{
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("search"))
			pControl->SelectItem(0);
		else if (name == _T("erasure"))
		{
			if (!m_ErasureLib)
			{
				m_ErasureLib = new CErasureLib();

			}
			CFramWnd* frame = m_ErasureLib->LibraryUI();
			_ASSERTE(frame);
			if (pControl->GetItemIndex(*frame) == -1)
			{
				AddVirtualWnd(frame->GetUI()->GetVirtualWnd(), frame);
				pControl->Add(*frame);
			}
			pControl->SelectItem(*frame);
		}
		else if (name == _T("protecte"))
		{
			if (!m_ProtectLib)
			{
				m_ProtectLib = new CProtectLib();
			}
			CFramWnd* frame = m_ProtectLib->LibraryUI();
			_ASSERTE(frame);
			if (pControl->GetItemIndex(*frame) == -1)
			{
				AddVirtualWnd(frame->GetUI()->GetVirtualWnd(), frame);
				pControl->Add(*frame);
			}
			pControl->SelectItem(*frame);
		}
		else if (name == _T("recove"))
		{
			CDlgGetFileName dlg;
			dlg.OpenFile(m_hWnd);
		}
		else if (name == _T("cleaner"))
			pControl->SelectItem(4);
	}

}

void CMainWnd::OnItemClick(TNotifyUI & msg)
{
}

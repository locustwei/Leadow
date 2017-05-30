#include "stdafx.h"
#include "MainWnd.h"


CMainWnd::CMainWnd(TCHAR* xmlSkin)
{
	m_Skin = xmlSkin;
// 	AddVirtualWnd(_T("page1"), &m_Page1);
// 	AddVirtualWnd(_T("page2"), &m_Page2);
}


CMainWnd::~CMainWnd()
{
}

DuiLib::CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode /*= NULL*/)
{
	return NULL;
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("selectchanged"))
	{
		CDuiString name = msg.pSender->GetName();
		CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
		if (name == _T("search"))
			pControl->SelectItem(0);
		else if (name == _T("foucedelete"))
			pControl->SelectItem(1);
		else if (name == _T("protecte"))
			pControl->SelectItem(2);
		else if (name == _T("recove"))
			pControl->SelectItem(3);
		else if (name == _T("cleaner"))
			pControl->SelectItem(4);
	}
	return __super::Notify(msg);
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("LdFileManagerWndClass");
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

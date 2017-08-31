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
}

CMainWnd::~CMainWnd()
{
	if (m_ErasureLib)
	{
		delete m_ErasureLib;
		m_ErasureLib = nullptr;
	}

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

void CMainWnd::InitWindow()
{
	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
	if (!pControl)
		return;

	m_ErasureLib = CLdLibray::LoadEraserLarary(&m_PaintManager);
	if (m_ErasureLib)
	{
		CFramNotifyPump* frame = m_ErasureLib->GetNotifyPump();
		if (frame)
		{
			AddVirtualWnd(frame->GetName(), frame);
		}
	}
	else
		return;
	pControl->Add(m_ErasureLib->GetUI());
	pControl->SelectItem(m_ErasureLib->GetUI());
}

void CMainWnd::OnSelectChanged(TNotifyUI & msg)
{

}

void CMainWnd::OnItemClick(TNotifyUI & msg)
{
}

/*
主窗口
*/
#include "stdafx.h"
#include "MainWnd.h"
#include "About.h"


DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(TCHAR* xmlSkin)
	:WindowImplBase()
{
	m_Skin = xmlSkin;
//	m_ErasureLib = NULL;
}

CMainWnd::~CMainWnd()
{
	/*if (m_ErasureLib)
	{
		CLdLibray::FreeErasureLibrary();
		m_ErasureLib = nullptr;
	}*/

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
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
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
	if (msg.pSender == m_btnLogo)
	{
		//About();
		CMessageDlg::MessageBox(_T("abcd"), _T("1234565677676"), MB_OK, 0, m_hWnd);
		return;
	}
	return __super::OnClick(msg);
}

CControlUI * BuildXml(TCHAR * skinXml)
{
	if (skinXml == nullptr || _tcslen(skinXml) == 0)
		return nullptr;

	CDialogBuilder builder;
	CPaintManagerUI pm_ui;
	CControlUI * pControl = builder.Create(skinXml, nullptr, NULL, &pm_ui);
	_ASSERTE(pControl);

	return pControl;
}

void CMainWnd::InitWindow()
{

	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
	if (!pControl)
		return;
	//嵌入文件擦除模块窗口
	m_EraserUI = new CErasureMainWnd();
	CControlUI* control = BuildXml(_T("erasure/erasuremain.xml"));
	m_EraserUI->AttanchControl(control);
	AddVirtualWnd(m_EraserUI->GetName(), m_EraserUI);
	pControl->Add(control);
	pControl->SelectItem(control);

	m_btnLogo = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("logo")));
}

void CMainWnd::About()
{
	CAbout About(_T("filemanager/about.xml"));
	About.Create(m_hWnd, _T("About"), UI_WNDSTYLE_FRAME, NULL);
	About.CenterWindow();
	About.ShowModal();
}

void CMainWnd::OnSelectChanged(TNotifyUI & msg)
{

}

void CMainWnd::OnItemClick(TNotifyUI & msg)
{
}

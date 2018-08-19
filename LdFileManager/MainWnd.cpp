/*
Ö÷´°¿Ú
*/
#include "stdafx.h"
#include "MainWnd.h"
#include "About.h"
#include "../plugins/plugin.h"
#include <LdPlugin.h>

DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(TCHAR* xmlSkin)
	:WindowImplBase()
	, m_btnLogo(nullptr)
	, m_EraserUI(nullptr)
{
	m_Skin = xmlSkin;
}

CMainWnd::~CMainWnd()
{

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
		About();
		return;
	}
	return __super::OnClick(msg);
}

void CMainWnd::InitWindow()
{

	CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switch")));
	if (!pControl)
		return;

	CLdString s = ThisApp->GetAppPath();
	s += PLUGIN_PATH;
	CPluginManager pm(s);
	CLdArray<PLUGIN_PROPERTY> plugins;
	pm.FindPlugin(PLUGIN_USAGE_UI, &plugins);

	for(int i=0; i<plugins.GetCount(); i++)
	{
		IPluginInterface* pi_interface = pm.LoadPlugin(ThisApp, plugins[i].id);
		m_EraserUI = pi_interface->CreateUI();
		if (!m_EraserUI)
		{
			delete pi_interface;
			continue;
		}
		AddVirtualWnd(m_EraserUI->GetName(), m_EraserUI);
		pControl->Add(m_EraserUI->GetUI());
		pControl->SelectItem(m_EraserUI->GetUI());
	}

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

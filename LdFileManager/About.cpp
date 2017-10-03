#include "stdafx.h"
#include "About.h"


DUI_BEGIN_MESSAGE_MAP(CAbout, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CAbout::CAbout(TCHAR* xmlSkin):
	WindowImplBase()
{
	m_Skin = xmlSkin;
}

CAbout::~CAbout()
{

}

void CAbout::Notify(TNotifyUI& msg)
{
	return __super::Notify(msg);
}

LPCTSTR CAbout::GetWindowClassName() const
{
	return _T("DyDotAbout");
}

DuiLib::CDuiString CAbout::GetSkinFile()
{
	return m_Skin;
}

CDuiString CAbout::GetSkinFolder()
{
	return _T("skin");
}

void CAbout::OnClick(TNotifyUI& msg)
{
	return __super::OnClick(msg);
}

void CAbout::InitWindow()
{

}

void CAbout::OnSelectChanged(TNotifyUI & msg)
{

}

void CAbout::OnItemClick(TNotifyUI & msg)
{

}

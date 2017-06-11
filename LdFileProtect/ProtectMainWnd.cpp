#include "stdafx.h"
#include "ProtectMainWnd.h"



CProtectMainWnd::CProtectMainWnd()
{
	BuildXml(_T("protecte\\chooseprotectflag.xml"), NULL);
}


CProtectMainWnd::~CProtectMainWnd()
{
}

void CProtectMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	MessageBox(0, msg.sType, NULL, 0);
}

void CProtectMainWnd::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CProtectMainWnd, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CProtectMainWnd::OnClick(TNotifyUI& msg)
{
	MessageBox(0, msg.sType, NULL, 0);
}

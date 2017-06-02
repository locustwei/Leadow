#include "stdafx.h"
#include "ErasureMainWnd.h"



CErasureMainWnd::CErasureMainWnd()
{
}


CErasureMainWnd::~CErasureMainWnd()
{
}

void CErasureMainWnd::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureMainWnd::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureMainWnd, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureMainWnd::OnClick(TNotifyUI& msg)
{

}

#include "stdafx.h"
#include "ErasureFileUI.h"



CErasureFileUI::CErasureFileUI()
{
	BuildXml(_T("erasure\\filelistview.xml"), NULL);
}


CErasureFileUI::~CErasureFileUI()
{
}

void CErasureFileUI::OnSelectChanged(TNotifyUI & msg)
{
	MessageBox(0, msg.sType, NULL, 0);
}

void CErasureFileUI::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("openfile"))
	{
		
	}
	else if (name == _T("btnOk"))
	{
	}
	else if (name == _T("unusedspace"))
	{

	}
	else if (name == _T("truemove"))
	{

	}
}

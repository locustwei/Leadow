#include "stdafx.h"
#include "ErasureOptions.h"


CErasureOptionsUI::CErasureOptionsUI()
{
	m_Name = _T("ErasureOptionsUI");
}


CErasureOptionsUI::~CErasureOptionsUI()
{
}

void CErasureOptionsUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureOptionsUI::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureOptionsUI, CFramNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()
void CErasureOptionsUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
}



void CErasureOptionsUI::OnClick(TNotifyUI& msg)
{
	
}

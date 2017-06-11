#include "stdafx.h"
#include "ErasureMainWnd.h"
#include "ErasureFileUI.h"


CErasureMainWnd::CErasureMainWnd()
{
	m_ErasureFile = NULL;
	BuildXml(_T("erasure\\erasuremain.xml"), NULL);
}


CErasureMainWnd::~CErasureMainWnd()
{
}

void CErasureMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("Shredder"))
	{
		if (m_ErasureFile == NULL)
		{
			m_ErasureFile = new CErasureFileUI();
			AddVirtualWnd(m_ErasureFile->GetUI()->GetVirtualWnd(), m_ErasureFile);
			m_TabUI->Add(m_ErasureFile->GetUI());
		}
		m_TabUI->SelectItem(m_ErasureFile->GetUI());
	}
	else if (name == _T("recycle"))
	{
	}
	else if (name == _T("unusedspace"))
	{
		
	}
	else if (name == _T("truemove"))
	{

	}
}

void CErasureMainWnd::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureMainWnd, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureMainWnd::OnInit()
{
	m_TabUI = static_cast<CTabLayoutUI*>(((CContainerUI*)m_Control)->FindSubControl(_T("pagelist")));
}

void CErasureMainWnd::OnClick(TNotifyUI& msg)
{
}

#include "stdafx.h"
#include "ErasureFileUI.h"
#include "ErasureRecycleUI.h"
#include "ErasureMainWnd.h"


CErasureMainWnd::CErasureMainWnd()
{
	m_ErasureFile = nullptr;
	m_ErasureRecycle = nullptr;
	BuildXml(_T("erasure\\erasuremain.xml"), nullptr);
}


CErasureMainWnd::~CErasureMainWnd()
{
	if (m_ErasureFile)
		delete m_ErasureFile;	
}

void CErasureMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("Shredder"))
	{
		if (m_ErasureFile == nullptr)
		{
			m_ErasureFile = new CErasureFileUI();
			AddVirtualWnd(m_ErasureFile->GetUI()->GetVirtualWnd(), m_ErasureFile);
			m_TabUI->Add(m_ErasureFile->GetUI());
		}
		m_TabUI->SelectItem(m_ErasureFile->GetUI());
	}
	else if (name == _T("recycle"))
	{
		if (m_ErasureRecycle == nullptr)
		{
			m_ErasureRecycle = new CErasureRecycleUI();
			AddVirtualWnd(m_ErasureRecycle->GetUI()->GetVirtualWnd(), m_ErasureRecycle);
			m_TabUI->Add(m_ErasureRecycle->GetUI());
		}
		m_TabUI->SelectItem(m_ErasureRecycle->GetUI());
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
	if (m_ErasureFile == nullptr)
	{
		m_ErasureFile = new CErasureFileUI();
		AddVirtualWnd(m_ErasureFile->GetUI()->GetVirtualWnd(), m_ErasureFile);
		m_TabUI->Add(m_ErasureFile->GetUI());
	}
	m_TabUI->SelectItem(m_ErasureFile->GetUI());
}

void CErasureMainWnd::OnClick(TNotifyUI& msg)
{
}

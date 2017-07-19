#include "stdafx.h"
#include "ErasureMainWnd.h"
//#include "ErasureFileUI.h"
#include "ErasureRecycleUI.h"
//#include "ErasureVolumeUI.h"

CErasureMainWnd::CErasureMainWnd()
{
	m_ErasureFile = nullptr;
	m_ErasureRecycle = nullptr;
	m_ErasureVolume = nullptr;
}


CErasureMainWnd::~CErasureMainWnd()
{
	if (m_ErasureRecycle)
		delete m_ErasureRecycle;

}

void CErasureMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("Shredder"))
	{
		if (m_ErasureFile == nullptr)
		{
//			m_ErasureFile = new CErasureFileUI();
//			AddVirtualWnd(m_ErasureFile->GetUI()->GetVirtualWnd(), m_ErasureFile);
//			m_TabUI->Add(m_ErasureFile->GetUI());
		}
//		m_TabUI->SelectItem(m_ErasureFile->GetUI());
	}
	else if (name == _T("recycle"))
	{
		if (m_ErasureRecycle == nullptr)
		{
			m_ErasureRecycle = new CErasureRecycleUI();
			AddVirtualWnd(m_ErasureRecycle->GetUI()->GetVirtualWnd(), m_ErasureRecycle);
			m_TabUI->Add(m_ErasureRecycle->GetUI());
			((CErasureRecycleUI*)m_ErasureRecycle)->AddFolder(CSIDL_BITBUCKET);
		}
		m_TabUI->SelectItem(m_ErasureRecycle->GetUI());
	}
	else if (name == _T("unusedspace"))
	{
		if (m_ErasureVolume == nullptr)
		{
//			m_ErasureVolume = new CErasureVolumeUI();
//			AddVirtualWnd(m_ErasureVolume->GetUI()->GetVirtualWnd(), m_ErasureVolume);
//			m_TabUI->Add(m_ErasureVolume->GetUI());
		}
//		m_TabUI->SelectItem(m_ErasureVolume->GetUI());
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
	m_TabUI = static_cast<CTabLayoutUI*>(m_Control->FindControl(CDuiUtils::FindControlByNameProc, _T("pagelist"), 0));
	if (m_ErasureFile == nullptr)
	{
//		m_ErasureFile = new CErasureFileUI();
//		AddVirtualWnd(m_ErasureFile->GetUI()->GetVirtualWnd(), m_ErasureFile);
//		m_TabUI->Add(m_ErasureFile->GetUI());
	}
//	m_TabUI->SelectItem(m_ErasureFile->GetUI());
}

void CErasureMainWnd::OnClick(TNotifyUI& msg)
{
}

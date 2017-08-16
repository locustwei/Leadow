#include "stdafx.h"
#include "ErasureMainWnd.h"
#include "ErasureFileUI.h"
#include "ErasureRecycleUI.h"
#include "ErasureVolumeUI.h"

//#include "ErasureVolumeUI.h"

CErasureMainWnd::CErasureMainWnd(): 
	m_TabUI(nullptr)
{
	m_ErasureFile = nullptr;
	m_ErasureRecycle = nullptr;
	m_ErasureVolume = nullptr;
	m_Name = _T("ErasureMainWnd");
}


CErasureMainWnd::~CErasureMainWnd()
{
	if (m_ErasureRecycle)
		delete m_ErasureRecycle;
	if (m_ErasureVolume)
		delete m_ErasureVolume;
}

void CErasureMainWnd::OnSelectChanged(TNotifyUI & msg)
{
	CDuiString name = msg.pSender->GetName();
	if (name == _T("Shredder"))
	{
		if (m_ErasureFile == nullptr)
		{
			m_ErasureFile = new CErasureFileUI();
			CControlUI* pCtrl = m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ErasureFile"), 0);
			m_ErasureFile->AttanchControl(pCtrl);
			AddVirtualWnd(m_ErasureFile->GetName(), m_ErasureFile);
		}
		m_TabUI->SelectItem(0);
	}
	else if (name == _T("recycle"))
	{
		if (m_ErasureRecycle == nullptr)
		{
			m_ErasureRecycle = new CErasureRecycleUI();
			CControlUI* pCtrl = m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ErasureRecycle"), 0);
			m_ErasureRecycle->AttanchControl(pCtrl);
			AddVirtualWnd(m_ErasureRecycle->GetName(), m_ErasureRecycle);
		}
		m_TabUI->SelectItem(1);
	}
	else if (name == _T("unusedspace"))
	{
		if (m_ErasureVolume == nullptr)
		{
			m_ErasureVolume = new CErasureVolumeUI();
			CControlUI* pCtrl = m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ErasureVolume"), 0);
			m_ErasureVolume->AttanchControl(pCtrl);
			AddVirtualWnd(m_ErasureVolume->GetName(), m_ErasureVolume);
		}
		m_TabUI->SelectItem(2);
	}
	else if (name == _T("truemove"))
	{

	}
}

void CErasureMainWnd::OnItemClick(TNotifyUI & msg)
{
}

void CErasureMainWnd::AttanchControl(CControlUI * pCtrl)
{
	__super::AttanchControl(pCtrl);
	m_TabUI = (CTabLayoutUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("pagelist"), 0);

	if (m_ErasureFile == nullptr)
	{
		m_ErasureFile = new CErasureFileUI();
		CControlUI* pCtrl = m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ErasureFile"), 0);
		m_ErasureFile->AttanchControl(pCtrl);
		AddVirtualWnd(m_ErasureFile->GetName(), m_ErasureFile);
	}

	m_TabUI->SelectItem(0);
}

DUI_BEGIN_MESSAGE_MAP(CErasureMainWnd, CFramNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()


void CErasureMainWnd::OnClick(TNotifyUI& msg)
{
}

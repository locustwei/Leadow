#include "stdafx.h"
#include "SelectProtectFlag.h"
#include "LdStructs.h"

CSelectProtectFlag::CSelectProtectFlag(TCHAR* xmlSkin):CLdDuiWnd(xmlSkin)
{
}


CSelectProtectFlag::~CSelectProtectFlag()
{
}

DWORD CSelectProtectFlag::SelectFlags(HWND hParentWnd)
{
	DWORD ret = 0;

	CSelectProtectFlag pFrame(_T("protecte\\chooseprotectflag.xml"));

	pFrame.Create(hParentWnd, _T(""), UI_WNDSTYLE_DIALOG, 0L);
	pFrame.CenterWindow();

	if (pFrame.ShowModal() == IDOK)
		ret = pFrame.m_Flags;

	return ret;
}

void CSelectProtectFlag::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK) {
		if (msg.pSender == m_btnOk) {
			m_Flags = 0;
			if (m_ckHide && m_ckHide->GetCheck())
				m_Flags |= LD_FILE_PROTECT::LFP_HIDE;

			if (m_ckRead && m_ckRead->GetCheck())
				m_Flags |= LD_FILE_PROTECT::LFP_READ;

			if (m_ckWrite && m_ckWrite->GetCheck())
				m_Flags |= LD_FILE_PROTECT::LFP_WRITE;

			if (m_ckRename && m_ckRename->GetCheck())
				m_Flags |= LD_FILE_PROTECT::LFP_RENAME;

			if (m_ckDelete && m_ckDelete->GetCheck())
				m_Flags |= LD_FILE_PROTECT::LFP_DELETE;

			PostMessage(WM_CLOSE, IDOK, 0);

			return;
		}
		else if (msg.pSender == m_btnCancel) {
			m_Flags = 0;
			Close();

			return;
		}
	}

	return __super::Notify(msg);
}

LRESULT CSelectProtectFlag::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LRESULT ret = __super::OnCreate(uMsg, wParam, lParam, bHandled);

	m_btnOk = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnOk")));
	m_btnCancel = static_cast<CButtonUI*>(GetPaintManager()->FindControl(_T("btnCancel")));

	m_ckHide = static_cast<CCheckBoxUI*>(GetPaintManager()->FindControl(_T("ckHide")));
	if (m_ckHide)
		m_ckHide->SetCheck(true, false);

	m_ckRead = static_cast<CCheckBoxUI*>(GetPaintManager()->FindControl(_T("ckRead")));
	if (m_ckRead)
		m_ckRead->SetCheck(true, false);

	m_ckWrite = static_cast<CCheckBoxUI*>(GetPaintManager()->FindControl(_T("ckWrite")));
	if (m_ckWrite)
		m_ckWrite->SetCheck(true, false);

	m_ckRename = static_cast<CCheckBoxUI*>(GetPaintManager()->FindControl(_T("ckRename")));
	if (m_ckRename)
		m_ckRename->SetCheck(true, false);

	m_ckDelete = static_cast<CCheckBoxUI*>(GetPaintManager()->FindControl(_T("ckDelete")));
	if (m_ckDelete)
		m_ckDelete->SetCheck(true, false);

	return ret;
}

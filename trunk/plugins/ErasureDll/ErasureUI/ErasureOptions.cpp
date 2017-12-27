#include "stdafx.h"
#include "ErasureOptions.h"
//#include "../ErasureImpl.h"


CErasureOptionsUI::CErasureOptionsUI(): 
	m_cbFile(nullptr)
	, m_cbVolume(nullptr)
	, m_ckFolder(nullptr)
	, m_ckSkipSpace(nullptr)
	, m_ckSkipTrack(nullptr)
	, m_ckFileFree(nullptr)
	, m_ckShutdown(nullptr)
	, m_btnSave(nullptr)
	, m_btnCommand(nullptr)
	, m_edtFile(nullptr)
	, m_edtRecycle(nullptr)
	, m_edtVolume(nullptr)
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

void CErasureOptionsUI::LoadConfig()
{
//	int k = ThisLibrary->GetConfig()->GetFileErasureMothed();
//	if (m_cbFile)
//		m_cbFile->SelectItem(k, false);
//	BOOL b = ThisLibrary->GetConfig()->IsRemoveFolder();
//	if(m_ckFolder)
//		m_ckFolder->SetCheck(b==TRUE, false);
//	k = ThisLibrary->GetConfig()->GetVolumeErasureMethed();
//	if (m_cbVolume)
//		m_cbVolume->SelectItem(k);
//	b = ThisLibrary->GetConfig()->IsSkipSpace();
//	if (m_ckSkipSpace)
//		m_ckSkipSpace->SetCheck(b==TRUE, false);
//	b = ThisLibrary->GetConfig()->IsSkipTrack();
//	if (m_ckSkipTrack)
//		m_ckSkipTrack->SetCheck(b == TRUE, false);
//	b = ThisLibrary->GetConfig()->IsErasureFreeFileSpace();
//	if (m_ckFileFree)
//		m_ckFileFree->SetCheck(b == TRUE, false);
//	b = ThisLibrary->GetConfig()->IsShutDown();
//	if (m_ckShutdown)
//		m_ckShutdown->SetCheck(b == TRUE, false);
}

DUI_BEGIN_MESSAGE_MAP(CErasureOptionsUI, CFramNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureOptionsUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	m_cbFile =      static_cast<CComboUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("cbFileMothed"), 0));
	m_cbVolume =    static_cast<CComboUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("cbVolumeMothed"), 0));
	m_ckFolder =    static_cast<CCheckBoxUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ckNoRemoveFolder"), 0));
	m_ckSkipSpace = static_cast<CCheckBoxUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ckSkipSpace"), 0));
	m_ckSkipTrack = static_cast<CCheckBoxUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ckSkipTrack"), 0));
	m_ckFileFree =  static_cast<CCheckBoxUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ckFileFree"), 0));
	m_ckShutdown =  static_cast<CCheckBoxUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("ckShutdown"), 0));
	m_btnSave =     static_cast<CButtonUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnSave"), 0));
	m_btnCommand =  static_cast<CButtonUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnCommand"), 0));
	m_edtFile =     static_cast<CEditUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("edtFile"), 0));
	m_edtRecycle =  static_cast<CEditUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("edtRecycle"), 0));
	m_edtVolume =   static_cast<CEditUI*>(pCtrl->FindControl(CDuiUtils::FindControlByNameProc, _T("edtVolume"), 0));

	LoadConfig();
}


void CErasureOptionsUI::SaveConfig()
{
//	if (m_cbFile)
//		ThisLibrary->GetConfig()->SetFileErasureMothed((ErasureMothedType)m_cbFile->GetCurSel());
//	if (m_ckFolder)
//		ThisLibrary->GetConfig()->SetRemoveFolder(m_ckFolder->GetCheck());
//	if (m_cbVolume)
//		ThisLibrary->GetConfig()->SetVolumeErasureMethed((ErasureMothedType)m_cbVolume->GetCurSel());
//	if (m_ckSkipSpace)
//		ThisLibrary->GetConfig()->SetSkipSpace(m_ckSkipSpace->GetCheck());
//	if (m_ckSkipTrack)
//		ThisLibrary->GetConfig()->SetSkipTrack(m_ckSkipTrack->GetCheck());
//	if (m_ckFileFree)
//		ThisLibrary->GetConfig()->SetErasureFreeFileSpace(m_ckFileFree->GetCheck());
//	if (m_ckShutdown)
//		ThisLibrary->GetConfig()->SetShutDown(m_ckShutdown->GetCheck());
//
//	ThisLibrary->GetConfig()->SaveConfig();
}

void CErasureOptionsUI::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_btnSave)
		SaveConfig();
}

#include "stdafx.h"
#include "FramNotifyPump.h"

namespace DuiLib {
	CFramNotifyPump::CFramNotifyPump()
		: m_Ctrl(nullptr)
	{
	}

	void CFramNotifyPump::AttanchControl(CControlUI * pCtrl)
	{
		m_Ctrl = pCtrl;
		if (m_Ctrl)
			m_Ctrl->SetVirtualWnd(m_Name);
	}

	CControlUI* CFramNotifyPump::GetUI()
	{
		return m_Ctrl;
	}

	CControlUI * CFramNotifyPump::BuildXml(TCHAR * skinXml)
	{
		if (skinXml == nullptr || _tcslen(skinXml) == 0)
			return nullptr;

		CDialogBuilder builder;
		CPaintManagerUI pm_ui;
		CControlUI * pControl = builder.Create(skinXml, nullptr, NULL, &pm_ui);
		_ASSERTE(pControl);

		return pControl;
	}

	DUI_BEGIN_MESSAGE_MAP(CFramNotifyPump, CNotifyPump)
	DUI_END_MESSAGE_MAP()

}

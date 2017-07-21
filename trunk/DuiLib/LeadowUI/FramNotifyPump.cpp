#include "stdafx.h"
#include "FramNotifyPump.h"

namespace DuiLib {
	CFramNotifyPump::CFramNotifyPump()
	{
	}

	void CFramNotifyPump::AttanchControl(CControlUI * pCtrl)
	{
		m_Ctrl = pCtrl;
		if (m_Ctrl)
			m_Ctrl->SetVirtualWnd(m_Name);
	}

	DUI_BEGIN_MESSAGE_MAP(CFramNotifyPump, CNotifyPump)
	DUI_END_MESSAGE_MAP()

}

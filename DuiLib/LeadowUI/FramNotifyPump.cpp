#include "stdafx.h"
#include "FramNotifyPump.h"

namespace DuiLib {
	CFramNotifyPump::CFramNotifyPump()
		: m_Ctrl(nullptr)
//		, m_Listener(nullptr)
	{
	}

	void CFramNotifyPump::AttanchControl(CControlUI * pCtrl)
	{
		m_Ctrl = pCtrl;
		if (m_Ctrl)
			m_Ctrl->SetVirtualWnd(m_Name);
//		if (m_Listener)
//			m_Listener->OnAttanch(pCtrl);
	}

	DUI_BEGIN_MESSAGE_MAP(CFramNotifyPump, CNotifyPump)
	DUI_END_MESSAGE_MAP()

}

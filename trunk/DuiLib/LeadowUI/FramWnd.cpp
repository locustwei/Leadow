#include "stdafx.h"
#include "FramWnd.h"

namespace DuiLib {
	CFramWnd::CFramWnd()
	{
		m_Control = NULL;
	}


	CFramWnd::~CFramWnd()
	{
	}

	DUI_BEGIN_MESSAGE_MAP(CFramWnd, CNotifyPump)
	DUI_END_MESSAGE_MAP()

		
	CControlUI * CFramWnd::BuildXml(TCHAR * skinXml, CPaintManagerUI* pm)
	{
		CPaintManagerUI paint;

		CDialogBuilder builder;
		m_Control = builder.Create(skinXml, (UINT)0, NULL, pm ? pm : &paint);
		_ASSERTE(m_Control);
		m_Control->SetVirtualWnd(skinXml);

		OnInit();

		return m_Control;
	}

	void CFramWnd::OnInit()
	{
	}
	

}

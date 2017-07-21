#pragma once

namespace DuiLib {
	class DUILIB_API CFramNotifyPump :public CNotifyPump
	{
	public:
		CFramNotifyPump();
		virtual void AttanchControl(CControlUI* pCtrl);
		DUI_DECLARE_MESSAGE_MAP()
	protected:
		CControlUI* m_Ctrl;
	};

}


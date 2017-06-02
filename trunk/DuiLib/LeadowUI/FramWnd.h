#pragma once

namespace DuiLib {
	class DUILIB_API CFramWnd :public CNotifyPump, public CControlUI
	{
	public:
		CFramWnd();
		~CFramWnd();

		DUI_DECLARE_MESSAGE_MAP()
	private:
		CControlUI* m_Control;
	};

}


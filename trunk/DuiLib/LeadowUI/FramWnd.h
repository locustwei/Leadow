#pragma once

namespace DuiLib {
	class DUILIB_API CFramWnd :public CNotifyPump
	{
	public:
		CFramWnd();
		~CFramWnd();

		CControlUI * BuildXml(TCHAR * skinXml, CPaintManagerUI* pm);
		operator CControlUI *() { return m_Control; };

		CControlUI * GetUI() { return m_Control; };

		DUI_DECLARE_MESSAGE_MAP()
	protected:
		CControlUI* m_Control;
		virtual void OnInit();
	};

}


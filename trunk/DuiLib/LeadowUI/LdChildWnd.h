#pragma once

namespace DuiLib {
	class DUILIB_API CLdChildWnd :public CNotifyPump, public IDialogBuilderCallback
	{
	public:
		CLdChildWnd();
		~CLdChildWnd();
		CControlUI* BuildXml(TCHAR* skinXml, CPaintManagerUI* pm);

		virtual CControlUI* CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode = NULL) override;
		operator CControlUI * ()
		{
			return m_Control;
		};

		DUI_DECLARE_MESSAGE_MAP()
	private:
		CControlUI* m_Control;
	};

}


#pragma once

namespace DuiLib {

	class DUILIB_API CLdUINotify : public INotifyUI, public IDialogBuilderCallback
	{
		friend class CLdDuiWnd;

	public:
		LPTSTR m_SkinXml;
		CPaintManagerUI m_pm;
		CWindowWnd* m_LdWnd;
		TCHAR m_ClassName[20];

		CLdUINotify();
		~CLdUINotify() {};

	protected:
		virtual LPCTSTR GetWndClassName();
		virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;
		virtual void Notify(TNotifyUI& msg) override;
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	};

	class DUILIB_API CLdDuiWnd : public CWindowWnd
	{
	public:
		CLdDuiWnd(CLdUINotify* pNotify);
		~CLdDuiWnd();
	private:
		CLdUINotify* m_Notify;

		void OnFinalMessage(HWND /*hWnd*/) { delete this; };
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		virtual LPCTSTR GetWindowClassName() const override;
		virtual UINT GetClassStyle() const override;
	};

	DUILIB_API BOOL CreateMainWnd(
		HINSTANCE hInstance,
		LPCTSTR lpResourcePath,
		CLdUINotify* pNotify,
		LPCTSTR lpAppTitle,
		int nCmdShow = SW_SHOW);
}
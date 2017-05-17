#pragma once

namespace DuiLib {

	class DUILIB_API CLdDuiWnd : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback
	{
	public:
		CLdDuiWnd(TCHAR* xmlSkin);
		~CLdDuiWnd();
		void SetSkinXml(LPCTSTR lpXml);
		LPCTSTR GetSkinXml();
		CPaintManagerUI* GetPaintManager();
	private:
		LPCTSTR m_SkinXml;
		CPaintManagerUI* m_PaintManager;
		TCHAR m_ClassName[20];

		void OnFinalMessage(HWND /*hWnd*/);
		void AfterWndZoomed(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		virtual UINT GetClassStyle() const override;
		virtual LPCTSTR GetWindowClassName() const override;
		virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;
		virtual void Notify(TNotifyUI& msg) override;
		virtual void Init();

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
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
	};

	DUILIB_API BOOL CreateMainWnd(
		HINSTANCE hInstance,
		LPCTSTR lpResourcePath,
		CLdDuiWnd* pWnd,
		LPCTSTR lpAppTitle,
		int nCmdShow = SW_SHOW);
}
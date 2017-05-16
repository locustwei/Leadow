#pragma once

namespace DuiLib
{
	class CLdMenu : public CWindowWnd, public INotifyUI
	{
	public:
		CLdMenu();
		~CLdMenu();

		void Init(CControlUI* pOwner, POINT pt);

		void AdjustPostion();

		LPCTSTR GetWindowClassName() const;;
		void OnFinalMessage(HWND /*hWnd*/) { delete this; };

		void Notify(TNotifyUI& msg);

		HWND Create(HWND hwndParent, LPCTSTR pstrName, 
			DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, 
			int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);

		void ShowWindow(bool bShow = true, bool bTakeFocus = true);

		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		CPaintManagerUI m_pm;
		CControlUI* m_pOwner;
		POINT m_ptPos;
		CWindowWnd* m_pShadowWnd;

	};


}

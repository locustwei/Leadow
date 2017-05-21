#pragma once
#include "LdMenu.h"

namespace DuiLib {

	class DUILIB_API CLdDuiWnd : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback, public INotifyMenu
	{
	public:
		CLdDuiWnd(TCHAR* xmlSkin);
		~CLdDuiWnd();
		void SetSkinXml(LPCTSTR lpXml);
		LPCTSTR GetSkinXml();
		CPaintManagerUI* GetPaintManager();
		CLdMenu* GetPopupMenu(LPCTSTR szMenuname);
	private:
		LPCTSTR m_SkinXml;
		CPaintManagerUI* m_PaintManager;
		TCHAR m_ClassName[20];
		CDuiStringPtrMap m_Menus;

		void OnFinalMessage(HWND /*hWnd*/);
		void AfterWndZoomed(UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		virtual UINT GetClassStyle() const override;
		virtual LPCTSTR GetWindowClassName() const override;
		virtual CControlUI* CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode) override;
		virtual void Notify(TNotifyUI& msg) override;
		virtual void Init();

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		virtual HWND GetWndHandle() override;
		virtual void OnPopup(CLdMenu* pMenu) override;
		virtual void OnMenuItemClick(CLdMenu* pMenu, int id) override;
	};

	DUILIB_API BOOL CreateMainWnd(
		HINSTANCE hInstance,
		LPCTSTR lpResourcePath,
		CLdDuiWnd* pWnd,
		LPCTSTR lpAppTitle,
		int nCmdShow = SW_SHOW);
}
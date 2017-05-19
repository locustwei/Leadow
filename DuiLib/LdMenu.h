#pragma once

namespace DuiLib
{
	class CMenuElementUI : public CListContainerElementUI
	{
	public:
		CMenuElementUI();
		~CMenuElementUI();

	protected:
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);

	};

	class CSubMenuUI: public CMenuElementUI
	{
	public:
		CSubMenuUI();
		~CSubMenuUI();

	private:

	};

	class CMenuLineUI: public CListElementUI
	{
	public:
		CMenuLineUI();
		~CMenuLineUI();

	private:

	};

	class CMenuUI : public CListUI
	{
	public:
		CMenuUI();
		~CMenuUI();
		virtual bool Add(CMenuElementUI* pMenuItem, int iIndex = -1);

		virtual bool Remove(CMenuElementUI* pMenuItem);
	protected:
		SIZE EstimateSize(SIZE szAvailable);
		virtual void DoEvent(TEventUI& event);
		LPVOID GetInterface(LPCTSTR pstrName);
		LPCTSTR GetClass() const;

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	// MenuElementUI
	extern const TCHAR* const kMenuElementUIClassName;// = _T("MenuElementUI");
	extern const TCHAR* const kMenuElementUIInterfaceName;// = _T("MenuElement);

	class CMenuWnd : public CLdDuiWnd
	{
	public:
		CMenuWnd(TCHAR* xmlSkin);
		LPCTSTR GetWindowClassName() const;
		void OnFinalMessage(HWND hWnd);

	public:
		HWND m_hParent;
		POINT m_BasedPoint;
		CMenuUI* m_Menu;
	protected:
		virtual CControlUI* CreateControl(LPCTSTR pstrClass) override;


		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;


		virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

	};


}

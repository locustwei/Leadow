#include "stdafx.h"
#include "LdMenu.h"

namespace DuiLib {

	// MenuUI
	const TCHAR* const kMenuUIClassName = _T("MenuUI");
	const TCHAR* const kMenuUIInterfaceName = _T("Menu");

	CMenuUI::CMenuUI()
	{
		if (GetHeader() != NULL)
			GetHeader()->SetVisible(false);
	}

	CMenuUI::~CMenuUI()
	{

	}

	LPCTSTR CMenuUI::GetClass() const
	{
		return kMenuUIClassName;
	}

	LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, kMenuUIInterfaceName) == 0) return static_cast<CMenuUI*>(this);
		return CListUI::GetInterface(pstrName);
	}

	void CMenuUI::DoEvent(TEventUI& event)
	{
		return __super::DoEvent(event);
	}

	bool CMenuUI::Add(CMenuElementUI* pMenuItem, int iIndex)
	{
		if (pMenuItem == NULL)
			return false;

		return CListUI::Add(pMenuItem);
	}

	bool CMenuUI::Remove(CMenuElementUI* pMenuItem)
	{
		if (pMenuItem == NULL)
			return false;

		return __super::Remove(pMenuItem);
	}

	SIZE CMenuUI::EstimateSize(SIZE szAvailable)
	{
		int cxFixed = 0;
		int cyFixed = 0;
		for (int it = 0; it < GetCount(); it++) {
			CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
			if (!pControl->IsVisible()) continue;
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;
			if (cxFixed < sz.cx)
				cxFixed = sz.cx;
		}
		return SIZE{ cxFixed, cyFixed };
	}

	void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		__super::SetAttribute(pstrName, pstrValue);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//

	CMenuWnd::CMenuWnd(TCHAR* xmlSkin): CLdDuiWnd(xmlSkin)
	{
		m_Menu = NULL;
	}

	LPCTSTR CMenuWnd::GetWindowClassName() const
	{
		return _T("MenuWnd");
	}

	void CMenuWnd::OnFinalMessage(HWND hWnd)
	{
		delete this;
	}

	CControlUI* CMenuWnd::CreateControl(LPCTSTR pstrClass)
	{
		if (_tcsicmp(pstrClass, kMenuUIInterfaceName) == 0)
		{
			if (!m_Menu)
				m_Menu = new CMenuUI();
			return m_Menu;
		}
		else if (_tcsicmp(pstrClass, kMenuElementUIInterfaceName) == 0)
		{
			return new CMenuElementUI();
		}
		return NULL;
	}

	LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case  WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
			return 0;
		case WM_KILLFOCUS:
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				Close();
			}
		default:
			break;
		}
		return __super::HandleMessage(uMsg, wParam, lParam);		
	}

	LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT ret = __super::OnCreate(uMsg, wParam, lParam, bHandled);
		
		SetForegroundWindow(m_hWnd);
		return ret;
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//

	// MenuElementUI
	const TCHAR* const kMenuElementUIClassName = _T("MenuElementUI");
	const TCHAR* const kMenuElementUIInterfaceName = _T("MenuElement");

	CMenuElementUI::CMenuElementUI() 
	{
		m_cxyFixed.cy = 25;
		m_bMouseChildEnabled = true;

		SetMouseChildEnabled(false);
	}

	CMenuElementUI::~CMenuElementUI()
	{}

	LPCTSTR CMenuElementUI::GetClass() const
	{
		return kMenuElementUIClassName;
	}

	LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, kMenuElementUIInterfaceName) == 0) 
			return static_cast<CMenuElementUI*>(this);
		return __super::GetInterface(pstrName);
	}

	void CMenuElementUI::DoEvent(TEventUI& event)
	{
		__super::DoEvent(event);
	}

	void CMenuElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		
		return __super::SetAttribute(pstrName, pstrValue);

	}

}
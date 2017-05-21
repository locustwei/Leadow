#include "stdafx.h"
#include "LdMenu.h"

namespace DuiLib {
	//CMenu::ActiveMenu = NULL;

	CLdMenu::CLdMenu()
	{
		m_hMenu = NULL;
		m_Notify = NULL;
		ZeroMemory(m_Name, ARRAYSIZE(m_Name) * sizeof(TCHAR));
	}

	CLdMenu::~CLdMenu()
	{
		if (m_hMenu)
			DestroyMenu(m_hMenu);
	}

	HMENU CLdMenu::Create(CMarkupNode * pNode)
	{
		if (m_hMenu || !pNode)
			return 0;

		pNode->GetAttributeValue(_T("name"), m_Name, 100);
		m_hMenu = CreatePopupMenu();
		UINT id = 1;
		for (CMarkupNode node = pNode->GetChild(); node.IsValid(); node = node.GetSibling())
		{
			TCHAR text[200] = { 0 };
			node.GetAttributeValue(_T("text"), text, 200);
			AppendMenu(m_hMenu, MF_STRING, id++, text);
		}
		//pNode->HasChildren()
		return m_hMenu;
	}
	LPCTSTR CLdMenu::GetName()
	{
		return m_Name;
	}
	UINT CLdMenu::Popup(INotifyMenu* pNotify, int x, int y)
	{
		if (!m_hMenu)
			return 0;
		if (x == -1 && y == -1)
		{
			POINT pt;
			GetCursorPos(&pt);
			x = pt.x;
			y = pt.y;
		}
		pNotify->OnPopup(this);
		UINT ret = TrackPopupMenuEx(m_hMenu, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, x, y, pNotify->GetWndHandle(), NULL);
		if (ret)
			pNotify->OnMenuItemClick(this, ret);
		return ret;
	}

	UINT CLdMenu::Popup(HWND hWnd, int x, int y)
	{
		if (!m_hMenu)
			return 0;
		if (x == -1 && y == -1)
		{
			POINT pt;
			GetCursorPos(&pt);
			x = pt.x;
			y = pt.y;
		}
		UINT ret = TrackPopupMenuEx(m_hMenu, TPM_LEFTALIGN | TPM_RETURNCMD | TPM_NONOTIFY, x, y, hWnd, NULL);
		return ret;
	}

}
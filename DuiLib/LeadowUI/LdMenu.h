#pragma once

#include "Component.h"

namespace DuiLib
{
	class CLdMenu;
	class CMenuItem
	{
	public:
		CMenuItem();
		~CMenuItem();

	private:

	};
	
	class DUILIB_API INotifyMenu
	{
	public:
		virtual HWND GetWndHandle() = 0;
		virtual void OnPopup(CLdMenu* pMenu) = 0;
		virtual void OnMenuItemClick(CLdMenu* pMenu, int id/*CMenuItem* pMenuItem*/) = 0;
	};

	class DUILIB_API CLdMenu : public CComponent
	{
	public:
		CLdMenu();
		~CLdMenu();

		HMENU Create(CMarkupNode* pNode);
		LPCTSTR GetName();
		void Popup(INotifyMenu* pNotify, int x = -1, int y = -1);
		
	protected:
		HMENU m_hMenu;
		INotifyMenu* m_Notify;
	private:
		TCHAR m_Name[100];
	};


}

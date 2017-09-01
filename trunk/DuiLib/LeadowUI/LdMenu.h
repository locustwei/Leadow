#pragma once


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

}

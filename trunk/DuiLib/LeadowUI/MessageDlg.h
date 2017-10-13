#pragma once
#include <Shlwapi.h>
#include <ShlObj.h>
#include "FramNotifyPump.h"

namespace DuiLib {

	class DUILIB_API CMessageDlg :
		public WindowImplBase
	{
	public:
		CMessageDlg();
		~CMessageDlg();

		static UINT MessageBox(
			TCHAR* lpTitle,
			TCHAR* lpMsg,
			DWORD nType,
			UINT nIcon,
			HWND hParent);
	private:
		CDuiString m_szMsg;
		CDuiString m_szTitle;
		DWORD m_nType;
		UINT m_nIcon;
	protected:
		LPCTSTR GetWindowClassName() const override;
		void InitWindow() override;
		LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
		CDuiString GetSkinFile() override;
		CDuiString GetSkinFolder() override;


		DUI_DECLARE_MESSAGE_MAP()

	};

}

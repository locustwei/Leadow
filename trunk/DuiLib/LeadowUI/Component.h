#pragma once

#define  LUI_COMP_MENU                            (_T("Menu"))
namespace DuiLib
{
	class DUILIB_API CComponent
	{
	public:
		CComponent();
		~CComponent();
		void SetAttribute(LPCTSTR name, LPCTSTR value);

	};

}
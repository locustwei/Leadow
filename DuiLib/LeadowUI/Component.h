#pragma once

#define  LUI_COMP_MENU                            (_T("Menu"))

class CComponent
{
public:
	CComponent();
	~CComponent();
	void SetAttribute(LPCTSTR name, LPCTSTR value);

};


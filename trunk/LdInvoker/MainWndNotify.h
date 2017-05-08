#pragma once
#include "..\DuiLib\LeadowUI\LdDuiWnd.h"

class CMainWndNotify : public CLdUINotify
{
public:
	CMainWndNotify();
	~CMainWndNotify();

	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

protected:
	virtual LPCTSTR GetWndClassName() override;
	virtual void Init() override;
};


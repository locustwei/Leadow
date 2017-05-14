#pragma once

class CProcessListView : public CLdUINotify
{
public:
	CProcessListView();
	~CProcessListView();
	static DWORD SelectFlags(HWND hParentWnd);
protected:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
};


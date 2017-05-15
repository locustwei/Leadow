#pragma once

class CProcessListView : public CLdDuiWnd
{
public:
	CProcessListView(TCHAR* xmlSkin);
	~CProcessListView();
	static DWORD SelectFlags(HWND hParentWnd);

	CListHeaderUI* AddHeader();
protected:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
private:
	CButtonUI* m_btnOk;
	CButtonUI* m_btnCancel;
	CListUI* m_List;
};


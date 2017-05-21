#pragma once

class CProcessListView : public CLdDuiWnd, public IListCallbackUI
{
public:
	CProcessListView(TCHAR* xmlSkin);
	~CProcessListView();

	BOOL AddHeader(CListHeaderItemUI* pHeaderItem, int nCol);
	CListHeaderItemUI* AddHeader(LPCTSTR lpText, UINT nFixedWidth, int nCol);
	int AddItem(CListElementUI* pListItem, int nIndex = -1);
	CListTextElementUI* AddTextItem();
	static BOOL KillProcess(HWND hParentWnd, PDWORD pids, UINT nCount);

protected:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual void OnMenuItemClick(CLdMenu* pMenu, int id/*CMenuItem* pMenuItem*/) override;
private:
	CButtonUI* m_btnOk;
	CButtonUI* m_btnCancel;
	CListUI* m_List;

	// Í¨¹ý IListCallbackUI ¼Ì³Ð
	virtual LPCTSTR GetItemText(CControlUI * pListItem, int iItem, int iSubItem) override;
};


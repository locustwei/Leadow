#pragma once

class CProcessListView : public CLdDuiWnd, public IListCallbackUI
{
public:
	CProcessListView(TCHAR* xmlSkin);
	~CProcessListView();
	static DWORD SelectFlags(HWND hParentWnd);

	BOOL AddHeader(CListHeaderItemUI* pHeaderItem, int nCol);
	CListHeaderItemUI* AddHeader(LPCTSTR lpText, UINT nFixedWidth, int nCol);
protected:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual void OnMenuItemClick(CLdMenu* pMenu, int id/*CMenuItem* pMenuItem*/) override;
private:
	CButtonUI* m_btnOk;
	CButtonUI* m_btnCancel;
	CListUI* m_List;

	// ͨ�� IListCallbackUI �̳�
	virtual LPCTSTR GetItemText(CControlUI * pList, int iItem, int iSubItem) override;
};


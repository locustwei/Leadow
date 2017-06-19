#pragma once


class LDLIB_API CErasureFileUI : public CFramWnd, public IListCallbackUI
{
public:
	CErasureFileUI();
	~CErasureFileUI();

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CListUI* lstFile;
	CLdArray<CFileInfo*> m_Files;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(CLdString& filename);
protected:
	virtual void OnInit() override;


	// ͨ�� IListCallbackUI �̳�
	virtual LPCTSTR GetItemText(CControlUI * pList, int iItem, int iSubItem) override;

};


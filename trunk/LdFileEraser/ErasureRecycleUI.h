#pragma once

class LDLIB_API CErasureRecycleUI : public CFramWnd, IRunable, IGernalCallback<LPWIN32_FIND_DATA>
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CListUI* lstFile;
//	CLdArray<CFileInfo*> m_Files;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(TCHAR* lpOrgFileName, LPWIN32_FIND_DATA pData);
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};


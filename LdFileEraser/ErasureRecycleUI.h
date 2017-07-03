#pragma once

class LDLIB_API CErasureRecycleUI : 
	public CFramWnd, 
	IRunable, 
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<CLdArray<TCHAR*>*>,  //回收站显示文件
	IGernalCallback<PSH_HEAD_INFO>,      //回收站文件显示列信息
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CListUI* lstFile;
	CLdArray<PSH_HEAD_INFO> m_Columes;
	CLdHashMap<LPWIN32_FIND_DATA> m_RecycledFiles;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(TCHAR* lpOrgFileName, LPWIN32_FIND_DATA pData);
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	void AddLstViewHeader(int ncount);
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;

	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};


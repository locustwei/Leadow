#pragma once

class LDLIB_API CErasureRecycleUI : 
	public CFramWnd, 
	IRunable, 
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<CLdArray<TCHAR*>*>,  //����վ��ʾ�ļ�
	IGernalCallback<PSH_HEAD_INFO>,      //����վ�ļ���ʾ����Ϣ
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
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


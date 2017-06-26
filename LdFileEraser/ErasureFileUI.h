#pragma once

class LDLIB_API CErasureFileUI : public CFramWnd, IRunable
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

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
	
	virtual VOID ThreadRun(WPARAM Param) override;
	virtual VOID OnThreadInit(WPARAM Param) override;
	virtual VOID OnThreadTerminated(WPARAM Param) override;

};


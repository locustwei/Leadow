#pragma once


class LDLIB_API CErasureMainWnd : public CFramWnd
{
public:
	CErasureMainWnd();
	virtual ~CErasureMainWnd() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	
	CTabLayoutUI* m_TabUI;
	CFramWnd* m_ErasureFile;
	CFramWnd* m_ErasureRecycle;
protected:
	virtual void OnInit() override;

};


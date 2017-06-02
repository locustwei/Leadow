#pragma once


class CErasureMainWnd : public CFramWnd
{
public:
	CErasureMainWnd();
	~CErasureMainWnd();

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


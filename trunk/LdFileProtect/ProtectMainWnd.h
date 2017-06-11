#pragma once


class LDLIB_API CProtectMainWnd : public CFramWnd
{
public:
	CProtectMainWnd();
	~CProtectMainWnd();

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


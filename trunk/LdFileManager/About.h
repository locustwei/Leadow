#pragma once

class CAbout : public WindowImplBase
{
public:
	CAbout(TCHAR* xmlSkin);
	~CAbout();

	DUI_DECLARE_MESSAGE_MAP()
protected:
	LPCTSTR GetWindowClassName() const override;
	void Notify(TNotifyUI& msg) override;
	CDuiString GetSkinFile() override;
	CDuiString GetSkinFolder() override;

	void OnClick(TNotifyUI& msg) override;
	void InitWindow() override;
private:
	TCHAR* m_Skin;

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


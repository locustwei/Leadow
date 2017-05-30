#pragma once


class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(TCHAR* xmlSkin);
	~CMainWnd();

	virtual CControlUI* CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode = NULL) override;



protected:
	virtual LPCTSTR GetWindowClassName() const override;
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual CDuiString GetSkinFile() override;
	virtual CDuiString GetSkinFolder() override;
private:
	TCHAR* m_Skin;

};


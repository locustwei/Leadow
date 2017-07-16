#pragma once
#include "Library.h"

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(TCHAR* xmlSkin);
	~CMainWnd();

	DUI_DECLARE_MESSAGE_MAP()
protected:
	virtual LPCTSTR GetWindowClassName() const override;
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual CDuiString GetSkinFile() override;
	//virtual CDuiString GetSkinFolder() override;

	virtual void OnClick(TNotifyUI& msg) override;
//	virtual CControlUI* CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode = NULL) override;

private:
	TCHAR* m_Skin;
	CErasureLib* m_ErasureLib;
	CProtectLib* m_ProtectLib;

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


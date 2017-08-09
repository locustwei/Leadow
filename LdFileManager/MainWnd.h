#pragma once
#include "Library.h"

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(TCHAR* xmlSkin);
	~CMainWnd();

	DUI_DECLARE_MESSAGE_MAP()
protected:
	LPCTSTR GetWindowClassName() const override;
	void Notify(TNotifyUI& msg) override;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	CDuiString GetSkinFile() override;
	CDuiString GetSkinFolder() override;

	void OnClick(TNotifyUI& msg) override;
	//virtual CControlUI* CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode = NULL) override;
	bool OnBtnAfterPaint(PVOID param);
private:
	TCHAR* m_Skin;
	IErasureLibrary* m_ErasureLib;
	IProtectLibrary* m_ProtectLib;

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


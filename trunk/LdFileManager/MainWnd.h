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
	void InitWindow() override;
private:
	TCHAR* m_Skin;
	IErasureLibrary* m_ErasureLib;
	CButtonUI* m_btnLogo;
	//CWndShadow m_WndShadow;

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


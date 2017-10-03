#pragma once

/*
程序主窗口，框架窗口嵌入功能模块窗口。
*/

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
	//IErasureLibrary* m_ErasureLib;  //文件擦除模块
	CButtonUI* m_btnLogo;
	CFramNotifyPump* m_EraserUI;

	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
};


#pragma once

class CSelectProtectFlag : public CLdUINotify
{
public:
	CSelectProtectFlag();
	~CSelectProtectFlag();
	
	DWORD m_Flags;

	static DWORD SelectFlags(HWND hParentWnd);
protected:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
private:
	CButtonUI* m_btnOk;
	CButtonUI* m_btnCancel;
	CCheckBoxUI* m_ckHide;
	CCheckBoxUI* m_ckRead;
	CCheckBoxUI* m_ckWrite;
	CCheckBoxUI* m_ckRename;
	CCheckBoxUI* m_ckDelete;
};


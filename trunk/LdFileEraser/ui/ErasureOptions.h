#pragma once

class CErasureOptionsUI : 
	public CFramNotifyPump
{
public:
	CErasureOptionsUI();
	~CErasureOptionsUI();

	DUI_DECLARE_MESSAGE_MAP()

private:
	CComboUI* m_cbFile;
	CComboUI* m_cbVolume;
	CCheckBoxUI* m_ckFolder;
	CCheckBoxUI* m_ckSkipSpace;
	CCheckBoxUI* m_ckSkipTrack;
	CCheckBoxUI* m_ckFileFree;
	CCheckBoxUI* m_ckShutdown;
	CButtonUI* m_btnSave;
	CButtonUI* m_btnCommand;
	CEditUI* m_edtFile;
	CEditUI* m_edtRecycle;
	CEditUI* m_edtVolume;
	void SaveConfig();
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg);
	void OnItemClick(TNotifyUI &msg);
protected:
	void LoadConfig();
	void AttanchControl(CControlUI* pCtrl) override;
};


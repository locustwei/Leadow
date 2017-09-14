#pragma once

class CErasureOptionsUI : 
	public CFramNotifyPump
{
public:
	CErasureOptionsUI();
	~CErasureOptionsUI();

	DUI_DECLARE_MESSAGE_MAP()

private:

	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg);
	void OnItemClick(TNotifyUI &msg);
protected:
	void AttanchControl(CControlUI* pCtrl) override;
};


#pragma once


class LDLIB_API CErasureMainWnd : public CFramNotifyPump
{
public:
	CErasureMainWnd();
	~CErasureMainWnd() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	
	CTabLayoutUI* m_TabUI;
	CFramNotifyPump* m_ErasureFile;
	CFramNotifyPump* m_ErasureRecycle;
	CFramNotifyPump* m_ErasureVolume;
protected:
	void AttanchControl(CControlUI* pCtrl) override;

};


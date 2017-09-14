#pragma once
/*
文件擦除主控页面。
*/

class CErasureMainWnd : public CFramNotifyPump
{
public:
	CErasureMainWnd();
	~CErasureMainWnd() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	
	CTabLayoutUI* m_TabUI;               //
	CFramNotifyPump* m_ErasureFile;      //文件擦除页
	CFramNotifyPump* m_ErasureRecycle;   //擦除回收站页
	CFramNotifyPump* m_ErasureVolume;    //磁盘擦除页
	CFramNotifyPump* m_ErasureOptions;   //擦除选项页
protected:
	void AttanchControl(CControlUI* pCtrl) override;

};


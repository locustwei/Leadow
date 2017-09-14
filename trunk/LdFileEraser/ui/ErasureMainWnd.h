#pragma once
/*
�ļ���������ҳ�档
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
	CFramNotifyPump* m_ErasureFile;      //�ļ�����ҳ
	CFramNotifyPump* m_ErasureRecycle;   //��������վҳ
	CFramNotifyPump* m_ErasureVolume;    //���̲���ҳ
	CFramNotifyPump* m_ErasureOptions;   //����ѡ��ҳ
protected:
	void AttanchControl(CControlUI* pCtrl) override;

};


#pragma once


namespace DuiLib {

	interface DUILIB_API IFrameListener
	{
		virtual void OnAttanch(CControlUI* pCtrl);
		virtual void OnClick(TNotifyUI& msg);
		virtual void OnSelectChanged(TNotifyUI &msg);
		virtual void OnItemClick(TNotifyUI &msg);
	};

	class DUILIB_API CFramNotifyPump 
		:public CNotifyPump
	{
	public:
		CFramNotifyPump();
		virtual void AttanchControl(CControlUI* pCtrl);
		DUI_DECLARE_MESSAGE_MAP()
	protected:
		CControlUI* m_Ctrl;
		IFrameListener* m_Listener;
	};

}


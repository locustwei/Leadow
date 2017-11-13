#pragma once


namespace DuiLib {

	interface DUILIB_API IFrameListener
	{
		virtual void OnAttanch(CControlUI* pCtrl) = 0;
		virtual void OnClick(TNotifyUI& msg) = 0;
		virtual void OnSelectChanged(TNotifyUI &msg) = 0;
		virtual void OnItemClick(TNotifyUI &msg) = 0;
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


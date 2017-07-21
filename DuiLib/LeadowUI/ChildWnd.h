#pragma once

namespace DuiLib
{
	class DUILIB_API CChildWnd
		: public CVerticalLayoutUI
		, public INotifyUI
	{
	public:
		CChildWnd();
		void Notify(TNotifyUI& msg) override;
	protected:
	public:
	};
}


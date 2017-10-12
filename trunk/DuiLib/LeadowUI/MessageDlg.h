#pragma once
#include <Shlwapi.h>
#include <ShlObj.h>

namespace DuiLib {

	class DUILIB_API CMessageDlg :
		public CFramNotifyPump
	{
	public:
		CMessageDlg();
		~CMessageDlg();
		void AttanchControl(CControlUI* pCtrl) override;
		
		DUI_DECLARE_MESSAGE_MAP()

	private:
	protected:
	};

}
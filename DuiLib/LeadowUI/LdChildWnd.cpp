#include "stdafx.h"
#include "LdChildWnd.h"

namespace DuiLib {
	CLdChildWnd::CLdChildWnd()
	{
		m_Control = NULL;
	}


	CLdChildWnd::~CLdChildWnd()
	{
	}

	DUI_BEGIN_MESSAGE_MAP(CLdChildWnd, CNotifyPump)
	DUI_END_MESSAGE_MAP()

	CControlUI * CLdChildWnd::BuildXml(TCHAR * skinXml, CPaintManagerUI* pm)
	{
		CDialogBuilder builder;
		CDuiString strResourcePath = pm->GetResourcePath();

// 		if (pm->GetResourceDll() == UILIB_RESOURCE)
// 		{
// 			STRINGorID xml(_ttoi(GetSkinFile().GetData()));
// 			pRoot = builder.Create(xml, _T("xml"), this, &m_PaintManager);
// 		}
// 		else
		m_Control = builder.Create(skinXml, (UINT)0, this, pm);

		return m_Control;
	}

	CControlUI* CLdChildWnd::CreateControl(LPCTSTR pstrClass, CMarkupNode* pNode /*= NULL*/)
	{
		return NULL;
	}

}

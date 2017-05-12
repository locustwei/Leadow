#include "stdafx.h"
#include "LdDuiWnd.h"

namespace DuiLib {

	CLdDuiWnd::CLdDuiWnd(CLdUINotify* pNotify)
	{
		m_Notify = pNotify;
		if (m_Notify)
			m_Notify->m_LdWnd = this;
	}


	CLdDuiWnd::~CLdDuiWnd()
	{
	}

	LRESULT CLdDuiWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BOOL bHandled = FALSE;

		if (m_Notify)
		{
			LRESULT ret = m_Notify->HandleMessage(uMsg, wParam, lParam, bHandled);
			if (bHandled)
				return ret;
		}
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LPCTSTR CLdDuiWnd::GetWindowClassName() const
	{
		return m_Notify->GetWndClassName();
	}
	
	UINT CLdDuiWnd::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}
	
	CLdUINotify::CLdUINotify()
	{
		ZeroMemory(m_ClassName, ARRAYSIZE(m_ClassName)*sizeof(TCHAR));
		wsprintf(m_ClassName, _T("LDWnd%8x"), (UINT)this);

		m_LdWnd = NULL;
		m_SkinXml = NULL;
	}

	void CLdUINotify::SetSkinXml(LPCTSTR lpXml)
	{
		m_SkinXml = lpXml;
	}

	LPCTSTR CLdUINotify::GetSkinXml()
	{
		return m_SkinXml;
	}

	CWindowWnd * CLdUINotify::GetWnd()
	{
		return m_LdWnd;
	}

	CPaintManagerUI * CLdUINotify::GetPaintManager()
	{
		return &m_pm;
	}

	LPCTSTR CLdUINotify::GetWndClassName()
	{
		return m_ClassName;
	}

	CControlUI * CLdUINotify::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	void CLdUINotify::Notify(TNotifyUI& msg)
	{
		CDuiString name = msg.pSender->GetName();
		if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		{
			Init();
			return;
		}

		if (msg.sType == DUI_MSGTYPE_CLICK) {
			if (name == _T("btnCloseWnd")) {
				m_LdWnd->Close();
				//SendMessage(m_LdWnd->GetHWND(), WM_CLOSE, 0, 0);
				return;
			}
			else if (name == _T("btnMinmizeWnd")) {
				SendMessage(m_LdWnd->GetHWND(), WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return;
			}
			else if (name == _T("btnMaxmizeWnd")) {
				SendMessage(m_LdWnd->GetHWND(), WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				return;
			}
			else if (name == _T("btnRestoreWnd")) {
				SendMessage(m_LdWnd->GetHWND(), WM_SYSCOMMAND, SC_RESTORE, 0);
				return;
			}
		}
	}

	void CLdUINotify::Init()
	{
	}

	LRESULT CLdUINotify::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		LONG styleValue = ::GetWindowLong(*m_LdWnd, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*m_LdWnd, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_LdWnd->GetHWND());
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(m_SkinXml, (UINT)0, this, &m_pm);
		if (pRoot)
		{
			m_pm.AttachDialog(pRoot);
			m_pm.AddNotifier(this);
		}
		return 0;
	}

	LRESULT CLdUINotify::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT CLdUINotify::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT CLdUINotify::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*m_LdWnd, &pt);

		RECT rcClient;
		::GetClientRect(*m_LdWnd, &rcClient);

		RECT rcCaption = m_pm.GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
			if (pControl && _tcscmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
				_tcscmp(pControl->GetClass(), DUI_CTR_OPTION) != 0)
			{
				bHandled = true;
				return HTCAPTION;
			}
		}
		bHandled = FALSE;
		return HTCLIENT;
	}

	LRESULT CLdUINotify::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdUINotify::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		LRESULT lRes = 0;
		switch (uMsg) {
		case WM_CREATE:
			lRes = OnCreate(uMsg, wParam, lParam, bHandled);
			break;
		case WM_CLOSE:
			lRes = OnClose(uMsg, wParam, lParam, bHandled);
			break;
		case WM_DESTROY:
			lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
			break;
		case WM_NCACTIVATE:
			lRes = OnNcActivate(uMsg, wParam, lParam, bHandled);
			break;
		case WM_NCCALCSIZE:
			lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
			break;
		case WM_NCPAINT:
			lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
			break;
		case WM_NCHITTEST:
			lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
			break;
		case WM_SIZE:
			lRes = OnSize(uMsg, wParam, lParam, bHandled);
			break;
		case WM_GETMINMAXINFO:
			lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
			break;
		case WM_SYSCOMMAND:
			lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);
			break;
		default:
			bHandled = FALSE;
		}
		if (bHandled)
			return lRes;
		if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
		{
			bHandled = true;
			return lRes;
		}
		return lRes;
	}

	DUILIB_API BOOL CreateMainWnd(
		HINSTANCE hInstance, 
		LPCTSTR lpResourcePath, 
		CLdUINotify* pNotify, 
		LPCTSTR lpAppTitle,
		int nCmdShow)
	{
		CPaintManagerUI::SetInstance(hInstance);
		CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + lpResourcePath);
		//CPaintManagerUI::SetResourceZip(_T("360SafeRes.zip"));

		HRESULT Hr = ::CoInitialize(NULL);
		if (FAILED(Hr)) 
			return FALSE;
		CLdDuiWnd* pFrame = new CLdDuiWnd(pNotify);
		pFrame->Create(NULL, lpAppTitle, UI_WNDSTYLE_FRAME, 0L);
		pFrame->CenterWindow();
		::ShowWindow(*pFrame, nCmdShow);

		CPaintManagerUI::MessageLoop();

		::CoUninitialize();
		return TRUE;
	}

}
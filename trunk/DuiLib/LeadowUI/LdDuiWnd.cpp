#include "stdafx.h"
#include "LdDuiWnd.h"

namespace DuiLib {

	CLdDuiWnd::CLdDuiWnd(TCHAR* xmlSkin)
	{
		m_PaintManager = new CPaintManagerUI();

		ZeroMemory(m_ClassName, ARRAYSIZE(m_ClassName) * sizeof(TCHAR));
		wsprintf(m_ClassName, _T("LDWnd%8x"), (UINT)this);

		m_SkinXml = xmlSkin;

	}


	CLdDuiWnd::~CLdDuiWnd()
	{
		m_PaintManager->RemoveNotifier(this);
		delete m_PaintManager;

		m_SkinXml = NULL;
	}

	void CLdDuiWnd::OnFinalMessage(HWND) 
	{ 
		//delete this; 
	}

	LRESULT CLdDuiWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		BOOL bHandled = FALSE;

		LRESULT lRes = 0;
		switch (uMsg) 
		{
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
		if (m_PaintManager->MessageHandler(uMsg, wParam, lParam, lRes))
		{
			bHandled = true;
			return lRes;
		}

		lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
 		if(uMsg == WM_SYSCOMMAND && wParam != SC_CLOSE)
 			AfterWndZoomed(uMsg, wParam, lParam);
		return lRes;
	}

	UINT CLdDuiWnd::GetClassStyle() const
	{
		return CS_DBLCLKS;
	}

	LPCTSTR CLdDuiWnd::GetWindowClassName() const 
	{
		return m_ClassName;
	}
	
	void CLdDuiWnd::SetSkinXml(LPCTSTR lpXml)
	{
		m_SkinXml = lpXml;
	}

	LPCTSTR CLdDuiWnd::GetSkinXml()
	{
		return m_SkinXml;
	}


	CPaintManagerUI * CLdDuiWnd::GetPaintManager()
	{
		return m_PaintManager;
	}

	void CLdDuiWnd::AfterWndZoomed(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		CControlUI* pControl;

		if (::IsZoomed(GetHWND()))
		{
			pControl = static_cast<CControlUI*>(m_PaintManager->FindControl(_T("btnMaxmizeWnd")));
			if (pControl)
				pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_PaintManager->FindControl(_T("btnRestoreWnd")));
			if (pControl)
				pControl->SetVisible(true);
		}
		else //if (wParam == SC_RESTORE)
		{
			pControl = static_cast<CControlUI*>(m_PaintManager->FindControl(_T("btnMaxmizeWnd")));
			if (pControl)
				pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_PaintManager->FindControl(_T("btnRestoreWnd")));
			if (pControl)
				pControl->SetVisible(false);
		}
	}

	CControlUI * CLdDuiWnd::CreateControl(LPCTSTR pstrClass)
	{
		return NULL;
	}

	void CLdDuiWnd::Notify(TNotifyUI& msg)
	{
		CDuiString name = msg.pSender->GetName();
		if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
		{
			Init();
			return;
		}

		if (msg.sType == DUI_MSGTYPE_CLICK) {
			if (name == _T("btnCloseWnd")) {
				Close();
				return;
			}
			else if (name == _T("btnMinmizeWnd")) {
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
				return;
			}
			else if (name == _T("btnMaxmizeWnd")) {
				SendMessage( WM_SYSCOMMAND, SC_MAXIMIZE, 0);
				return;
			}
			else if (name == _T("btnRestoreWnd")) {
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
				return;
			}
		}
	}

	void CLdDuiWnd::Init()
	{
	}

	LRESULT CLdDuiWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_PaintManager->Init(GetHWND());
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(m_SkinXml, (UINT)0, this, m_PaintManager);
		if (pRoot)
		{
			m_PaintManager->AttachDialog(pRoot);
			m_PaintManager->AddNotifier(this);
		}
		return 0;
	}

	LRESULT CLdDuiWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdDuiWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdDuiWnd::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT CLdDuiWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		bHandled = TRUE;
		return 0;
	}

	LRESULT CLdDuiWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdDuiWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(GetHWND(), &pt);

		RECT rcClient;
		::GetClientRect(GetHWND(), &rcClient);

		RECT rcCaption = m_PaintManager->GetCaptionRect();
		if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager->FindControl(pt));
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

	LRESULT CLdDuiWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdDuiWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		return 0;
	}

	LRESULT CLdDuiWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
	{
		
		return 0;
	}

	DUILIB_API BOOL CreateMainWnd(
		HINSTANCE hInstance, 
		LPCTSTR lpResourcePath, 
		CLdDuiWnd* pWnd,
		LPCTSTR lpAppTitle,
		int nCmdShow)
	{
		CPaintManagerUI::SetInstance(hInstance);
		CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + lpResourcePath);
		//CPaintManagerUI::SetResourceZip(_T("360SafeRes.zip"));

		HRESULT Hr = ::CoInitialize(NULL);
		if (FAILED(Hr)) 
			return FALSE;
		pWnd->Create(NULL, lpAppTitle, UI_WNDSTYLE_FRAME, 0L);
		pWnd->CenterWindow();
		::ShowWindow(pWnd->GetHWND(), nCmdShow);

		CPaintManagerUI::MessageLoop();

		::CoUninitialize();
		return TRUE;
	}

}
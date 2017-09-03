#include "stdafx.h"
//#include <Uxtheme.h>
//#include <vsstyle.h>
//#include <vssym32.h>

namespace DuiLib
{

#define WND_NC_WIDTH 1

//////////////////////////////////////////////////////////////////////////

LPBYTE WindowImplBase::m_lpResourceZIPBuffer=NULL;

DUI_BEGIN_MESSAGE_MAP(WindowImplBase,CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

void WindowImplBase::OnFinalMessage( HWND hWnd )
{
	m_PaintManager.RemovePreMessageFilter(this);
	m_PaintManager.RemoveNotifier(this);
	m_PaintManager.ReapObjects(m_PaintManager.GetRoot());
}

LRESULT WindowImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}

	return FALSE;
}

UINT WindowImplBase::GetClassStyle() const
{
	return CS_DBLCLKS;
}

UILIB_RESOURCETYPE WindowImplBase::GetResourceType() const
{
	return UILIB_FILE;
}

CDuiString WindowImplBase::GetZIPFileName() const
{
	return _T("");
}

LPCTSTR WindowImplBase::GetResourceID() const
{
	return _T("");
}

CControlUI* WindowImplBase::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

LRESULT WindowImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, bool& /*bHandled*/)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return ResponseDefaultKeyEvent(wParam);
		default:
			break;
		}
	}
	return FALSE;
}

LRESULT WindowImplBase::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)
LRESULT WindowImplBase::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if( ::IsIconic(*this) ) bHandled = FALSE;
	return (wParam == 0) ? TRUE : FALSE;
}

LRESULT WindowImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPRECT pRect=NULL;
	
	int borderwidth = WND_NC_WIDTH;

	if ( wParam == TRUE)
	{
		LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
		if (IsZoomed(m_hWnd))
		{
//			borderwidth = GetSystemMetrics(SM_CXSIZEFRAME);
//			pParam->rgrc[0].top = pParam->rgrc[0].top + borderwidth;
//			pParam->rgrc[0].left = pParam->rgrc[0].left + borderwidth;
//			pParam->rgrc[0].right = pParam->rgrc[0].right - borderwidth;
//			pParam->rgrc[0].bottom = pParam->rgrc[0].bottom - borderwidth; 
		}else
			pParam->rgrc[0].bottom = pParam->rgrc[0].bottom - borderwidth;  //加一像素，利用Window系统窗口阴影

	}
	else
	{
		//bHandled = false;
	}
	return 0;

}

/*
void DrawThemedFrame(HWND hWnd, HRGN hrgnUpdate, HTHEME hTheme)
{
	int idPart = EP_EDITBORDER_HVSCROLL;
	int idState = EPSHV_NORMAL;
	int cxEdge = GetSystemMetrics(SM_CXEDGE);
	int cyEdge = GetSystemMetrics(SM_CYEDGE);

	if (GetFocus() == hWnd)
	{
		idState = EPSHV_FOCUSED;
	}

	if (!IsWindowEnabled(hWnd))
	{
		idState = EPSHV_DISABLED;
	}

	HDC hDC = GetWindowDC(hWnd);
	if (hDC)
	{
		//
		// Get the border size from the theme
		int cxBorder, cyBorder;
		GetThemeInt(hTheme, idPart, idState, TMT_BORDERSIZE, &cxBorder);
		cyBorder = cxBorder;

		//
		// Get the window coordinates
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);

		//
		// Create an update region without the client edge to pass to 
		// DefWindowProc
		InflateRect(&rcWindow, -cxEdge, -cyEdge);
		HRGN hrgn = CreateRectRgnIndirect(&rcWindow);
		if (hrgnUpdate)
		{
			CombineRgn(hrgn, hrgnUpdate, hrgn, RGN_AND);
		}

		//
		// Zero origin the rect
		OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);

		//
		// Clip our drawing to the non-client edge
		OffsetRect(&rcWindow, cxEdge, cyEdge);
		ExcludeClipRect(hDC,
			rcWindow.left,
			rcWindow.top,
			rcWindow.right,
			rcWindow.bottom);
		InflateRect(&rcWindow, cxEdge, cyEdge);
		DrawThemeBackground(hTheme, hDC, idPart, idState, &rcWindow, 0);

		//
		// Fill with the control's brush since the theme border may not be as 
		// thick as the client edge
		if (cxBorder < cxEdge && cyBorder < cyEdge)
		{
			InflateRect(&rcWindow, cxBorder - cxEdge, cyBorder - cyEdge);
			FillRect(hDC, &rcWindow, GetSysColorBrush(COLOR_WINDOW));
		}
		//
		// Let DefWindowProc to the rest, excluding the border we just painted
		DefWindowProc(hWnd, WM_NCPAINT, (WPARAM)hrgn, 0);

		DeleteObject(hrgn);
		ReleaseDC(hWnd, hDC);
	}
}
*/

LRESULT WindowImplBase::OnNcPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	//todo  不能画到SizeBox，所有底边有一像素处理不到的东西
	return 0;

/*
	HDC hDC;
	if (wParam == 1)
		hDC = GetWindowDC(m_hWnd);
	else
		hDC = GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW | DCX_INTERSECTRGN | DCX_CACHE | DCX_CLIPSIBLINGS);
	if (hDC == 0)
		return 0;
	// Paint into this DC 
	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	OffsetRect(&rc, -rc.left, -rc.top);
	LOGBRUSH logBrush;
	logBrush.lbStyle = BS_SOLID;
	logBrush.lbColor = 0xFFFFFFFF;// RGB(0xFF, 0, 0);
	logBrush.lbHatch = HS_HORIZONTAL;
	HBRUSH hbrush = CreateBrushIndirect(&logBrush);
	if (wParam == 1)
	{
		RECT r = rc;
		r.bottom = r.top + WND_NC_WIDTH;
		CRenderEngine::DrawColor(hDC, r, 0xFEFF0000);
		//FillRect(hDC, &r, hbrush);
		r = rc;
		r.top = r.bottom - WND_NC_WIDTH;
		CRenderEngine::DrawColor(hDC, r, 0xFEFF0000);
		//FillRect(hDC, &r, hbrush);
		r = rc;
		r.right = r.left + WND_NC_WIDTH;
		//FillRect(hDC, &r, hbrush);
		CRenderEngine::DrawColor(hDC, r, 0xFEFF0000);
		r = rc;
		r.left = r.right - WND_NC_WIDTH;
		//FillRect(hDC, &r, hbrush);
		CRenderEngine::DrawColor(hDC, r, 0xFEFF0000);
	}
	else
		FillRgn(hDC, (HRGN)wParam, hbrush);

	DeleteObject(hbrush);


	ReleaseDC(m_hWnd, hDC);
	return 0;
*/
}

LRESULT WindowImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	
	if( !::IsZoomed(*this) )
	{
		RECT rcSizeBox = m_PaintManager.GetSizeBox();
		if( pt.y < rcClient.top + rcSizeBox.top )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
			return HTTOP;
		}
		else if( pt.y > rcClient.bottom - rcSizeBox.bottom )
		{
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
			return HTBOTTOM;
		}

		if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
		if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
	}

	RECT rcCaption = m_PaintManager.GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(pt));
			if( pControl && _tcsicmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 && 
				_tcsicmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
				_tcsicmp(pControl->GetClass(), DUI_CTR_TEXT) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT WindowImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	CDuiRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;

	lpMMI->ptMaxTrackSize.x =rcWork.GetWidth();
	lpMMI->ptMaxTrackSize.y =rcWork.GetHeight();

	lpMMI->ptMinTrackSize.x =m_PaintManager.GetMinInfo().cx;
	lpMMI->ptMinTrackSize.y =m_PaintManager.GetMinInfo().cy;

	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseWheel(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}
#endif

LRESULT WindowImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_PaintManager.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
#endif
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == SC_CLOSE)
	{
		bHandled = TRUE;
		SendMessage(WM_CLOSE);
		return 0;
	}
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed )
	{
        CControlUI* pbtnMax = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));         // max button
        CControlUI* pbtnRestore = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn"))); // restore button

        // toggle status of max and restore button
        if (pbtnMax && pbtnRestore)
        {
            pbtnMax->SetVisible(TRUE == bZoomed);
            pbtnRestore->SetVisible(FALSE == bZoomed);
        }
	}
#else
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
	return lRes;
}

LRESULT WindowImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_PaintManager.Init(m_hWnd);
	m_PaintManager.AddPreMessageFilter(this);

	CDialogBuilder builder;
	CDuiString strResourcePath=m_PaintManager.GetResourcePath();
	if (strResourcePath.IsEmpty())
	{
		strResourcePath=m_PaintManager.GetInstancePath();
		strResourcePath+=GetSkinFolder().GetData();
	}
	m_PaintManager.SetResourcePath(strResourcePath.GetData());

	switch(GetResourceType())
	{
	case UILIB_ZIP:
		m_PaintManager.SetResourceZip(GetZIPFileName().GetData(), true);
		break;
	case UILIB_ZIPRESOURCE:
		{
			HRSRC hResource = ::FindResource(m_PaintManager.GetResourceDll(), GetResourceID(), _T("ZIPRES"));
			if( hResource == NULL )
				return 0L;
			DWORD dwSize = 0;
			HGLOBAL hGlobal = ::LoadResource(m_PaintManager.GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
#if defined(WIN32) && !defined(UNDER_CE)
				::FreeResource(hResource);
#endif
				return 0L;
			}
			dwSize = ::SizeofResource(m_PaintManager.GetResourceDll(), hResource);
			if( dwSize == 0 )
				return 0L;
			m_lpResourceZIPBuffer = new BYTE[ dwSize ];
			if (m_lpResourceZIPBuffer != NULL)
			{
				::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
			}
#if defined(WIN32) && !defined(UNDER_CE)
			::FreeResource(hResource);
#endif
			m_PaintManager.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
		}
		break;
	}

	CControlUI* pRoot=NULL;
	if (GetResourceType()==UILIB_RESOURCE)
	{
		STRINGorID xml(_ttoi(GetSkinFile().GetData()));
		pRoot = builder.Create(xml, _T("xml"), this, &m_PaintManager);
	}
	else
		pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_PaintManager);
	ASSERT(pRoot);
	if (pRoot==NULL)
	{
		MessageBox(NULL,_T("加载资源文件失败"),_T("Duilib"),MB_OK|MB_ICONERROR);
		ExitProcess(1);
		return 0;
	}
	m_PaintManager.AttachDialog(pRoot);
	m_PaintManager.AddNotifier(this);

	InitWindow();
	return 0;
}

LRESULT WindowImplBase::OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT WindowImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch (uMsg)
	{
	case WM_CREATE:			lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
	case WM_CLOSE:			lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
	case WM_DESTROY:		lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)
	case WM_NCACTIVATE:		lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
	case WM_NCCALCSIZE:		lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
	//case WM_NCPAINT:		lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break; //让系统处理,才会有窗口阴影
	case WM_NCHITTEST:		lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
	case WM_GETMINMAXINFO:	lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEWHEEL:		lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;
#endif
	case WM_SIZE:			lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
	case WM_CHAR:		lRes = OnChar(uMsg, wParam, lParam, bHandled); break;
	case WM_SYSCOMMAND:		lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
	case WM_KEYDOWN:		lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
	case WM_KILLFOCUS:		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_SETFOCUS:		lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONUP:		lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;
	case WM_LBUTTONDOWN:	lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEMOVE:		lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;
	case WM_MOUSEHOVER:	lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;
	default:				bHandled = FALSE; break;
	}
	if (bHandled) return lRes;

	lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
	if (bHandled) return lRes;

	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes))
		return lRes;
	lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

	return lRes;
}

LRESULT WindowImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LONG WindowImplBase::GetStyle()
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;

	return styleValue;
}

void WindowImplBase::OnClick(TNotifyUI& msg)
{
	CDuiString sCtrlName = msg.pSender->GetName();
	if( sCtrlName == _T("closebtn") )
	{
		Close();
		return; 
	}
	else if( sCtrlName == _T("minbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("maxbtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}
	else if( sCtrlName == _T("restorebtn"))
	{ 
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
		return; 
	}
	return;
}

void WindowImplBase::Notify(TNotifyUI& msg)
{
	return CNotifyPump::NotifyPump(msg);
}

}
#include "stdafx.h"
#include "MessageDlg.h"

namespace DuiLib {

	CMessageDlg::CMessageDlg()
		: m_nType(0)
		, m_nIcon(0)
	{
	}
	CMessageDlg::~CMessageDlg()
	{
	}

	UINT CMessageDlg::MessageBox(TCHAR* lpTitle, TCHAR* lpMsg, DWORD nType, UINT nIcon, HWND hParent)
	{
		CMessageDlg dlg;
		dlg.m_szMsg = lpMsg;
		dlg.m_szTitle = lpTitle;
		dlg.m_nType = nType;
		dlg.m_nIcon = nIcon;
		dlg.Create(hParent, nullptr, UI_WNDSTYLE_DIALOG, 0);
		dlg.ShowModal();
		return 0;
	}

	DUI_BEGIN_MESSAGE_MAP(CMessageDlg, WindowImplBase)
	DUI_END_MESSAGE_MAP()

	LPCTSTR CMessageDlg::GetWindowClassName() const
	{
		return _T("LdMessageDialogBox");
	}

	void CMessageDlg::InitWindow()
	{
	}

	LRESULT CMessageDlg::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_PaintManager.Init(m_hWnd);
		int nLinks = 0;
		RECT rcText = { 0, 0, 9999, 9999 };

		CRenderEngine::DrawHtmlText(m_PaintManager.GetPaintDC(), &m_PaintManager, rcText, m_szMsg, 0, NULL, NULL, nLinks, 1, DT_CALCRECT | DT_WORDBREAK);

		int nClientX = rcText.right - rcText.left + 50;
		int nClientY = rcText.bottom - rcText.top + 100;

		if (nClientX < 240) nClientX = 240;
		if (nClientY < 130) nClientY = 130;
		m_PaintManager.SetInitSize(nClientX, nClientY);
		RECT rcCaption = { 0, 0, 0, 30 };
		m_PaintManager.SetCaptionRect(rcCaption);

		CVerticalLayoutUI* pRoot = new CVerticalLayoutUI;
		pRoot->SetAttributeList(m_PaintManager.GetStyleAttributeList(_T("hor_default")));

		//提示标题
		CHorizontalLayoutUI* pCaption = new CHorizontalLayoutUI;
		pCaption->SetFixedHeight(30);
		pRoot->Add(pCaption);

		CLabelUI* pTitleLabel = new CLabelUI;
		pTitleLabel->SetFixedHeight(30);
		pTitleLabel->SetAttributeList(m_PaintManager.GetStyleAttributeList(_T("txt_default")));
		pTitleLabel->SetText(m_szTitle);
		pCaption->Add(pTitleLabel);

		CButtonUI* btnClose = new CButtonUI;
		btnClose->SetAttributeList(m_PaintManager.GetStyleAttributeList(_T("btn_closebox")));
		btnClose->SetFixedWidth(45);
		pCaption->Add(btnClose);

		//msg 正文
		CTextUI* pMsgLabel = new CTextUI;
		pMsgLabel->SetName(_T("msg_text"));
		pMsgLabel->SetText(m_szMsg);
		pMsgLabel->SetShowHtml(true);
		pMsgLabel->SetAttributeList(m_PaintManager.GetStyleAttributeList(_T("txt_default")));
		pMsgLabel->SetAttribute(_T("align"), _T("center"));
		//pMsgLabel->EstimateSize();
		pRoot->Add(pMsgLabel);
		pRoot->Add(new CControlUI);
		//类型 按钮个数和功能
		CHorizontalLayoutUI* pBtnHor = new CHorizontalLayoutUI;
		pBtnHor->SetFixedHeight(40);
		pBtnHor->Add(new CControlUI);

		CButtonUI* btn;

		if(m_nType == MB_OK)
		{
			btn = new CButtonUI;
			btn->SetName(_T("okbtn"));
			btn->SetText(_T("确定"));
			btn->SetAttributeList(m_PaintManager.GetStyleAttributeList(_T("btn_default")));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);
		}
		if(m_nType & MB_YESNO)
		{
			btn = new CButtonUI;
			btn->SetName(_T("yesbtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);

			btn = new CButtonUI;
			btn->SetName(_T("nobtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);
		}
		if(m_nType & MB_RETRYCANCEL)
		{
			btn = new CButtonUI;
			btn->SetName(_T("retrybtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);

			btn = new CButtonUI;
			btn->SetName(_T("cancelbtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);
		}

		if(m_nType & MB_OKCANCEL)
		{
			btn = new CButtonUI;
			btn->SetName(_T("okbtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);

			btn = new CButtonUI;
			btn->SetName(_T("cancelbtn"));
			btn->SetText(_T("确定"));
			btn->SetAttribute(_T("style"), _T("btn_default"));
			btn->SetFixedWidth(60);
			btn->SetFixedHeight(30);
			pBtnHor->Add(btn);
		}


		m_PaintManager.AttachDialog(pRoot);
		m_PaintManager.AddNotifier(this);
		//m_PaintManager.SetBackgroundTransparent(TRUE);
		
		return 0;

	}

	CDuiString CMessageDlg::GetSkinFile()
	{
		return _T("");
	}

	CDuiString CMessageDlg::GetSkinFolder()
	{
		return _T("");
	}
}


#pragma once

class CChildView : public CWnd
{
public:
	CChildView(){};
	virtual ~CChildView(){};

	// ��д
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs)
	{
		if (!CWnd::PreCreateWindow(cs))
			return FALSE;

		cs.dwExStyle |= WS_EX_CLIENTEDGE;
		cs.style &= ~WS_BORDER;
		cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
			::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

		return TRUE;
	};
};


class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	CChildFrame(CWnd* wndView);
	virtual ~CChildFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// ʵ��
public:
	// ��ܹ���������ͼ��
	CWnd* m_wndView;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

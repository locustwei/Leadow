// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// LeadowStockHepler.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "LeadowStockHepler.h"
#include "MainFrm.h"
#include "..\StockDataAPI\web\HttpStockData.h"
#include "tdxinterface\TdxTrading.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeadowStockHeplerApp

BEGIN_MESSAGE_MAP(CLeadowStockHeplerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CLeadowStockHeplerApp::OnAppAbout)
END_MESSAGE_MAP()


// CLeadowStockHeplerApp ����

CLeadowStockHeplerApp::CLeadowStockHeplerApp()
{
	m_bHiColorIcons = TRUE;

	// TODO: ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Leadow.StockHepler.V0.1"));

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_bSaveState = FALSE;
}

// Ψһ��һ�� CLeadowStockHeplerApp ����

CLeadowStockHeplerApp theApp;

// CLeadowStockHeplerApp ��ʼ��

BOOL CLeadowStockHeplerApp::InitInstance()
{
	CWinAppEx::InitInstance();
	// ��ʼ�� OLE ��
// 	if (!AfxOleInit())
// 	{
// 		AfxMessageBox(IDP_OLE_INIT_FAILED);
// 		return FALSE;
// 	}

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	SetRegistryKey(_T("LeadowStockHepler"));

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	InitScriptEng();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame();
	if (!pFrame)
		return FALSE;
	pFrame->EnableLoadDockState(FALSE);

	m_pMainWnd = pFrame;
	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME);
	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}

int CLeadowStockHeplerApp::ExitInstance()
{
	//TODO: �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);

	delete m_ScriptEng;
	delete m_TradClient;
	delete m_DateInterface;

	return CWinAppEx::ExitInstance();
}

// CLeadowStockHeplerApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CLeadowStockHeplerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CLeadowStockHeplerApp �Զ������/���淽��

void CLeadowStockHeplerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_NAV);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_NAV);
}

void CLeadowStockHeplerApp::LoadCustomState()
{
}

void CLeadowStockHeplerApp::SaveCustomState()
{
}

//���ӽ��������Ŀǰֻ֧��ͨ���ţ�
ITradInterface* ConnectTradClient()
{
	return new CTdxTrading();
}

void CLeadowStockHeplerApp::InitScriptEng()
{
	m_ScriptEng = new CScriptEng();
	m_DateInterface = new CHttpStockData();
	m_TradClient = ConnectTradClient();
	m_ScriptEng->SetDataInterface(m_DateInterface);
	m_ScriptEng->SetTradInterface(m_TradClient);
}

// CLeadowStockHeplerApp ��Ϣ�������




#include "stdafx.h"
#include "MainWnd.h"
#include "Library.h"

#define APP_TITLE _T("Leadow File Manager")

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	if (!CLdApp::Initialize(hInstance))
		return 0;
	
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
	CWndShadow::Initialize(hInstance);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	CDialogBuilder builder;
	CPaintManagerUI paint;

	builder.Create(_T("default\\defualt.xml"), NULL, NULL, &paint, NULL);
	CMainWnd MainWnd(_T("filemanager\\filemanagemain.xml"));
	MainWnd.Create(NULL, APP_TITLE, UI_WNDSTYLE_FRAME, 0);
	MainWnd.CenterWindow();
	::ShowWindow(MainWnd, SW_SHOW);
 
	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.hwnd == NULL)
			CLdApp::MainThreadMessage(msg);
		if (!CPaintManagerUI::TranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	::CoUninitialize();
	return 0;
}


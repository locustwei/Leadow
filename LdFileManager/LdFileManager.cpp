#include "stdafx.h"
#include "MainWnd.h"

#define APP_TITLE _T("Leadow File Manager")

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CMainWnd MainWnd(_T("filemanagemain.xml"));
	MainWnd.Create(NULL, APP_TITLE, UI_WNDSTYLE_FRAME, 0);
	MainWnd.CenterWindow();
	::ShowWindow(MainWnd, SW_SHOW);
 
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}
/*
程序入口
*/

#include "stdafx.h"
#include "MainWnd.h"
#include "MainCommunication.h"
#include "../LdApp/LdApp.h"

#define APP_TITLE _T("DYDOT File Manager")

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	if (!CLdApp::Initialize(hInstance))
		return 0;
	CPaintManagerUI::SetInstance(hInstance);
#ifdef _DEBUG
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
#else  //正式版用加密ZIP
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
	CPaintManagerUI::SetResourceZip(_T("UIResource.zip"), true, "dydot1234");
#endif
	CDialogBuilder builder;
	CPaintManagerUI paint;
	
	builder.Create(_T("default/defualt.xml"), NULL, NULL, &paint, NULL);  //控件预定义样式
	CMainWnd MainWnd(_T("filemanager/filemanagemain.xml"));               //主窗口 
	MainWnd.Create(NULL, APP_TITLE, UI_WNDSTYLE_FRAME, WS_EX_APPWINDOW | WS_EX_OVERLAPPEDWINDOW);
	MainWnd.CenterWindow();
	::ShowWindow(MainWnd, SW_SHOW);

	CMainCommunication mc;
	mc.Listen();
 
	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.hwnd == NULL)                                 //线程消息（同步线程发消息到主线程实现代码同步）
			CLdApp::MainThreadMessage(msg);
		if (!CPaintManagerUI::TranslateMessage(&msg)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	::CoUninitialize();

	return 0;
}


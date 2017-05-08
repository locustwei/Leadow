/*!
 * \file LdInvoker.cpp
 * \date 2017/04/26 14:22
 *
 * asa
 * 
 * Invoker 用于加载并调用动态库函数。
 * Explorer 插件不能直接执行动态库函数（如需管理员权限）
 * 或为了避免插件占用太多资源，启动进程执行功能。
 * 
 *
 * 
*/

#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include "LdInvoker.h"
#include "MainWndNotify.h"

#include "LdStructs.h"
#include "LdFunction.h"

LD_FUNCTION_ID functionId = LFI_NONE;
LD_FUNCTION_FLAG functionFlag = LFF_NONE;

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

LPTSTR* ParamStrs = NULL;
int ParamCount = 0;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	HACCEL hAccelTable;
	
	ParamStrs = CommandLineToArgvW(lpCmdLine, &ParamCount);

	if(ParamCount < 2)
		return ERROR_PARAMCOUNT;

	hInst = hInstance;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LDINVOKER));
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LDINVOKER, szWindowClass, MAX_LOADSTRING);
	/*
	CMainWndNotify* pNotify = new CMainWndNotify();
	pNotify->SetSkinXml(_T("invokermainwnd.xml"));
	CreateMainWnd(hInstance, _T("skin"), pNotify, szTitle, SW_HIDE);

	delete pNotify;
	*/
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
	//CPaintManagerUI::SetResourceZip(_T("360SafeRes.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return FALSE;

	functionId = (LD_FUNCTION_ID)StrToInt(ParamStrs[0]);
	functionFlag = (LD_FUNCTION_FLAG)StrToInt(ParamStrs[1]);

	InvokeLdFunc(functionId, ParamStrs[2], NULL);

	::PostQuitMessage(0L);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	return (int) 0;
}

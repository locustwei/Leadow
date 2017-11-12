/*!
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
#include "Eraser/RunErasure.h"
#include "../LdApp/LdApp.h"

#pragma warning(disable:4996)


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	

	CLdStringW cmdLine = lpCmdLine;
	cmdLine.Trim();

	DebugOutput(cmdLine.GetData());

	if(cmdLine.IsEmpty())
	{
		//goto help
		return 0;
	}

	int ParamCount;
	
	LPWSTR* lpParamStrs = CommandLineToArgvW(cmdLine, &ParamCount);

	if(ParamCount==0)
	{
		//goto help;
		return 0;
	}

	CLdApp::Initialize(hInstance);

	if(wcsicmp(lpParamStrs[0], CMD_ERASE_FILE) == 0)
	{
		if(ParamCount < 2)
		{
			//goto help
			return 0;
		}
		if(RunEraseFile(&lpParamStrs[1], ParamCount - 1) != 0)
		{
			//goto help;
			//return 0;
		}
	}

	LocalFree(lpParamStrs);

	DebugOutput(L"exit");

	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.hwnd == NULL) 
			CLdApp::MainThreadMessage(msg);
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	

	return (int) 0;
}
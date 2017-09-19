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
#include <Shlwapi.h>
#include "LdInvoker.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	CLdStringW cmdLine = lpCmdLine;
	int ParamCount;
	LPWSTR* lpParamStrs = CommandLineToArgvW(cmdLine, &ParamCount);

	for(int i=0; i<ParamCount;i++)
	{
		MessageBox(0, lpParamStrs[i], nullptr, 0);
	}

	CLdApp::Initialize(hInstance);

	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.hwnd == NULL) 
			CLdApp::MainThreadMessage(msg);
	}

	PostQuitMessage(0);

	return (int) 0;
}

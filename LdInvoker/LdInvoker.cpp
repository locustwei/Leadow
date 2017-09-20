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

#define CMD_ERASE_FILE L"/erasefile"
#define CMD_ERASE_RECYCLE L"/eraserecycle"
#define CMD_ERASE_VOLUME L"/erasevolume"

bool AnalEraseFileParam(LPWSTR*, int);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	CLdStringW cmdLine = lpCmdLine;
	int ParamCount;
	
	LPWSTR* lpParamStrs = CommandLineToArgvW(cmdLine, &ParamCount);

	if(ParamCount==0)
	{
		//goto help;
		return 0;
	}

	if(wcsicmp(lpParamStrs[0], CMD_ERASE_FILE) == 0)
	{
		if(ParamCount < 2)
		{
			//goto help
			return 0;
		}
		if(!AnalEraseFileParam(&lpParamStrs[1], ParamCount - 1))
		{
			//goto help;
			return 0;
		}

		for(int i=1;i<ParamCount;i++)
		{
			
		}
	}

	for(int i=0; i<ParamCount;i++)
	{
		MessageBox(0, lpParamStrs[i], nullptr, 0);
	}

	LocalFree(lpParamStrs);

	CLdApp::Initialize(hInstance);

	MSG msg = { 0 };
	while (::GetMessage(&msg, NULL, 0, 0)) {
		if (msg.hwnd == NULL) 
			CLdApp::MainThreadMessage(msg);
	}

	PostQuitMessage(0);

	return (int) 0;
}

bool AnalEraseFileParam(LPWSTR* lpParams, int nParamCount)
{
#define MOTHED L"mothed"

	bool result = false;

	for(int i=0; i<nParamCount; i++)
	{
		if(wcsicmp(lpParams[i], MOTHED)==0)
		{
			
		}
	}

	return result;
}
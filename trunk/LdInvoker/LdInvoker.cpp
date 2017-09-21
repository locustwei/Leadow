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

bool AnalEraseFileParam(LPWSTR*, int, JsonBox::Value&);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);

	CLdStringW cmdLine = lpCmdLine;
	cmdLine.Trim();
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

	if(wcsicmp(lpParamStrs[0], CMD_ERASE_FILE) == 0)
	{
		if(ParamCount < 2)
		{
			//goto help
			return 0;
		}
		JsonBox::Value Param;
		if(!AnalEraseFileParam(&lpParamStrs[1], ParamCount - 1, Param))
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

int Try2Int(LPWSTR pstr, int Default)
{
	if (!pstr)
		return Default;
	else
	{
		try
		{
			return _wtoi(pstr);
		}
		catch (...)
		{
			return Default;
		}
	}
}

bool AnalEraseFileParam(LPWSTR* lpParams, int nParamCount, JsonBox::Value& Params)
{
#define MOTHED L"mothed"
#define FILE L"file"
#define UNDELFOLDER L"undelfolder"

	int mothed;
	for(int i=0; i<nParamCount; i++)
	{
		if(wcsnicmp(lpParams[i], MOTHED, wcslen(MOTHED))==0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			mothed = Try2Int(p, -1);
			if (mothed == -1)
				return false;
			Params["mothed"] = mothed;
		}else if (wcsnicmp(lpParams[i], FILE, wcslen(FILE)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			CLdStringA str = p;
			str.Trim();
			if (str.GetLength() < 3)
				return false;
			if(str[0]=='\"')
			{
				str.Delete(0, 1);
			}
			if (str[str.GetLength() - 1] == '\"')
				str.Delete(str.GetLength() - 1, 1);

			JsonBox::Array arr = Params["file"].getArray();
			arr.push_back(JsonBox::Value(str.GetData()));
			Params["file"] = arr;
		}else if (wcsnicmp(lpParams[i], UNDELFOLDER, wcslen(UNDELFOLDER)) == 0)
		{
			Params["delfolder"] = true;
		}
		else
			return false;
	}

	return true;
}
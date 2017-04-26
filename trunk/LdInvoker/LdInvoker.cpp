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
#include "LdInvoker.h"
#include <Shellapi.h>
#include <string>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

HMODULE g_Dll = 0;
LPTSTR* ParamStrs = NULL;
int ParamCount = 0;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

using namespace std;


//************************************
// Method:    LoadDll
// FullName:  LoadDll
// Access:    public 
// Returns:   HMODULE
// Qualifier: 加载动态库
// Parameter: LPTSTR DllName，参数传入动态库文件名不包含路径。
//************************************
HMODULE LoadDll(LPTSTR DllName)
{
	TCHAR FileName[MAX_PATH] = {0};
	if(!GetModuleFileName(0, FileName, MAX_PATH))
		return NULL;
	wstring s(FileName);
	int k = s.find_last_of('\\');
	if(k == -1)
		return NULL;
	s = s.substr(0, k + 1);
	s += DllName;
	return LoadLibrary(s.c_str());
}

typedef int (*_EntryFunction)(HWND hManWnd, LPTSTR ParamStr);

//************************************
// Method:    InvokeFunction
// FullName:  InvokeFunction
// Access:    public 
// Returns:   int
// Qualifier: 执行动态库入口函数。
// Parameter: HWND hWnd 主窗口句柄。
// Parameter: LPTSTR ParamStr 入口函数参数
//************************************
int InvokeFunction(HWND hWnd, LPTSTR ParamStr)
{
	_EntryFunction EntryFunction;
	EntryFunction = (_EntryFunction)GetProcAddress(g_Dll, "EntryFunction");
	if(EntryFunction)
		return EntryFunction(hWnd, ParamStr);
	else
		return ERROR_FUNCTION_NOT_FOUND;
	return 0;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;
	
	ParamStrs = CommandLineToArgvW(lpCmdLine, &ParamCount);

	if(ParamCount < 2)
		return ERROR_PARAMCOUNT;

	g_Dll = LoadDll(ParamStrs[0]);
	if(g_Dll == 0)
		return ERROR_LOAD_LIBRARY;

	if(ParamCount > 2)
	{//如果动态库函数没有窗口时
		if(ParamStrs[1][0] == '1'){
			return InvokeFunction(0, ParamStrs[2]);
		}
	}

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LDINVOKER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//nCmdShow = SW_HIDE;

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LDINVOKER));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LDINVOKER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LDINVOKER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		if(InvokeFunction(hWnd, ParamStrs[1]) == ERROR_FUNCTION_NOT_FOUND)
			PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

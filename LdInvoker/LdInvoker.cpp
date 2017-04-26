/*!
 * \file LdInvoker.cpp
 * \date 2017/04/26 14:22
 *
 * asa
 * 
 * Invoker ���ڼ��ز����ö�̬�⺯����
 * Explorer �������ֱ��ִ�ж�̬�⺯�����������ԱȨ�ޣ�
 * ��Ϊ�˱�����ռ��̫����Դ����������ִ�й��ܡ�
 * 
 *
 * 
*/

#include "stdafx.h"
#include "LdInvoker.h"
#include <Shellapi.h>
#include <string>

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

HMODULE g_Dll = 0;
LPTSTR* ParamStrs = NULL;
int ParamCount = 0;

// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

using namespace std;


//************************************
// Method:    LoadDll
// FullName:  LoadDll
// Access:    public 
// Returns:   HMODULE
// Qualifier: ���ض�̬��
// Parameter: LPTSTR DllName���������붯̬���ļ���������·����
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
// Qualifier: ִ�ж�̬����ں�����
// Parameter: HWND hWnd �����ھ����
// Parameter: LPTSTR ParamStr ��ں�������
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

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;
	
	ParamStrs = CommandLineToArgvW(lpCmdLine, &ParamCount);

	if(ParamCount < 2)
		return ERROR_PARAMCOUNT;

	g_Dll = LoadDll(ParamStrs[0]);
	if(g_Dll == 0)
		return ERROR_LOAD_LIBRARY;

	if(ParamCount > 2)
	{//�����̬�⺯��û�д���ʱ
		if(ParamStrs[1][0] == '1'){
			return InvokeFunction(0, ParamStrs[2]);
		}
	}

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LDINVOKER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	//nCmdShow = SW_HIDE;

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LDINVOKER));

	// ����Ϣѭ��:
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

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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

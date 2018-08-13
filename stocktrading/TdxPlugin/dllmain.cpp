// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "winds\TDXLogin.h"
#include "..\publiclib\Utils_Wnd.h"
#include "winds\TDXMain.h"
#include "..\publiclib\comps\NotifyIcon.h"
#include "..\publiclib\comps\LdList.h"
#include "TDXDataStruct.h"
#include <stdio.h>
#include "TdxTradSocket.h"

HINSTANCE hInstance;
DWORD dwViceThreadId;
CTdxTradSocket* TradSocket;
HWND hWndLogin = NULL;
HWND hMainWnd = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lparam)
{
	if(WndClassNameIs(hwnd, CN_Dialog)){
		DWORD style = GetWindowLong(hwnd, GWL_STYLE);
		if(style==0x94000044){
			hWndLogin = hwnd;
		}else if(style==0x0CCF0044)
			hMainWnd = hwnd;
	}
	return hWndLogin == NULL || hMainWnd == NULL;
}

WNDPROC oldProc;

//����ʱ��ʱʹ�ù��ӣ�����ע�����������߳�
LRESULT WINAPI TempWndPROC(HWND hwnd, UINT nCode,WPARAM wparam,LPARAM lparam)
{
	if(nCode==MM_RUNONMAINTHREAD){
		(WNDPROC)SetWindowLongPtr(hWndLogin, GW_WNDPROC, (LONG)oldProc); //����ʹ����ɻ�ԭ����
		((RunThreadFunc)wparam)(lparam);
	}
	return CallWindowProc(oldProc, hwnd, nCode, wparam, lparam);
}

//���̣߳�����߳����ڴ����������߳��е�ģ̬�Ի���ѭ���ȴ���������
DWORD WINAPI ViceThreadProc(_In_ LPVOID lpParameter)
{
	MSG msg;
	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if(msg.message == WM_QUIT)
			break;
		else if(msg.wParam!=0){
			RunThreadFunc fun = (RunThreadFunc)msg.wParam;
			fun(msg.lParam);
		}
	}

	return 0;
}

//�����̳߳���ʼ����
void InitOnMainThread(LPARAM param)
{

	if(hWndLogin!=NULL){
		CTDXLogin::WndHooker = new CTDXLogin(hWndLogin);
		CTDXLogin::WndHooker->StartHook();
	}

	if(hMainWnd!=NULL){
		CTDXMain::WndHooker = new CTDXMain(hMainWnd);
		CTDXMain::WndHooker->StartHook();

	}

	CreateThread(NULL, 0, &ViceThreadProc, NULL, 0, &dwViceThreadId);

	TradSocket = new CTdxTradSocket();

}

BOOL InstallHooks(DWORD tid)
{
	EnumThreadWindows(tid, &EnumThreadWndProc, 0);
	if(hWndLogin!=NULL){
		if(tid == GetCurrentThreadId())
			InitOnMainThread(0);
		else{
			/*��ǰ�̲߳������̣߳����������߳����������ڵȶ�����������߳�ͬ�����⡣
			�������ҵ���¼����������Ϣ���ӣ�Ȼ������Ϣ�����߳�ִ�г�ʼ������*/

			oldProc = (WNDPROC)SetWindowLongPtr(hWndLogin, GW_WNDPROC, (LONG)&TempWndPROC);
			PostMessage(hWndLogin, MM_RUNONMAINTHREAD, (WPARAM)&InitOnMainThread, 0);
		}
		return TRUE;
	}else
		return FALSE;
}

void UnInstallHooks()
{
}
//-------------------------------------------------------------------------------------------------------------------------------------
//����ע�뷽ʽ��̬�⵼��������
extern "C" int __declspec( dllexport ) RunPlugin(DWORD pid)
{
	InstallHooks(pid);

	
	while(true)   //�̲߳��ܽ������������
		Sleep(10);

	return 0;
}
//------------------------------------------------------------------------------------------------------------------------------------
typedef int (* Addin_)(); 
INT_PTR timerid;

VOID CALLBACK TimerProc(
	__in  HWND hwnd,
	__in  UINT uMsg,
	__in  UINT_PTR idEvent,
	__in  DWORD dwTime
	)
{
	if(InstallHooks(GetCurrentThreadId()))
		KillTimer(NULL, timerid);
}
//ͨ���Ų����ʽ��������
extern "C" int __declspec( dllexport ) Addin_GetObject()
{
	 //����Dll�ǳ���ո��������ڻ�û���������ö�ʱ���ȴ���
	timerid = SetTimer(NULL, 123, 100, &TimerProc); 


	HMODULE h = LoadLibrary(L"AddinCommonControl.dll");

	Addin_ p = (Addin_)GetProcAddress(h, "Addin_GetObject");

	int o = p();

	return o;
}
//------------------------------------------------------------------------------------------------------------------------------------
//������
#ifdef _DEBUG
extern "C" void __declspec( dllexport ) PlugTest()
{
	InitOnMainThread(0);
}
#endif
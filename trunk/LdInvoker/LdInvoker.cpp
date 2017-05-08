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
#include <shellapi.h>
#include <Shlwapi.h>
#include "LdInvoker.h"
#include "MainWndNotify.h"

#include "LdStructs.h"
#include "LdFunction.h"

LD_FUNCTION_ID functionId = LFI_NONE;
LD_FUNCTION_FLAG functionFlag = LFF_NONE;

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

LPTSTR* ParamStrs = NULL;
int ParamCount = 0;


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
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

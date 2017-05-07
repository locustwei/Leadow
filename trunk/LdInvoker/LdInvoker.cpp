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
#include "LdFunction.h"
#include "MainWndNotify.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

LD_FUNCTION_ID functionId = LFI_NONE;
LD_FUNCTION_FLAG functionFlag = LFF_NONE;

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

	if(ParamCount != 2)
		return ERROR_PARAMCOUNT;

	functionId = (LD_FUNCTION_ID)_wtoi(ParamStrs[0]);
	functionFlag = (LD_FUNCTION_FLAG)_wtoi(ParamStrs[1]);

	InvokeLdFunc(functionId, (LD_FUNCTION_FLAG)(functionFlag ^ LFF_NEW_PROCESS), NULL);
	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LDINVOKER, szWindowClass, MAX_LOADSTRING);

	//nCmdShow = SW_HIDE;
	hInst = hInstance;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LDINVOKER));
	
	CMainWndNotify* pNotify = new CMainWndNotify();
	pNotify->m_SkinXml = _T("skin.xml");
	CreateMainWnd(hInstance, _T("skin"), pNotify, _T("Leadow Window"), SW_SHOW);
	
	delete pNotify;

	return (int) 0;
}

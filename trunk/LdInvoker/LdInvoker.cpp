/*!
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
#include <LdStructs.h>
#include "../plugins/plugin.h"

#pragma warning(disable:4996)


DWORD LoadPlugin(TCHAR* plugid)
{
	DebugOutput(L"LoadPlugin plugid=%s", plugid);

	CPluginManager pm(ThisApp->GetPluginPath());
	IPluginInterface* pi = pm.LoadPlugin(ThisApp, plugid);
	if (pi == nullptr)
		return 1;
	return pi->InitCommunicate();
}

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

	CLdApp::Initialize(hInstance);
	DebugOutput(cmdLine.GetData());

	TCHAR* plugid = nullptr;
	for(int i=0; i<ParamCount; i++)
	{
		if(_tcsstr(lpParamStrs[i], HOST_PARAM_PLUGID) == lpParamStrs[i])
		{
			plugid = lpParamStrs[i] + _tcslen(HOST_PARAM_PLUGID);
//			if (pipename != nullptr)
			LoadPlugin(plugid);
			continue;
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

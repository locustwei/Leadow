#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include "UIlib.h"
#include "LdLib.h"
#include "LdList.h"

/*
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return FALSE;
	DWORD ids[] = {12848, 6552, 3160, 6400};
	CProcessListView::KillProcess(NULL, ids, ARRAYSIZE(ids));

	::PostQuitMessage(0L);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	return (int) 0;
}

BOOL EnumProcessModalCallback_(PMODULEENTRY32 pEntry32, PVOID pParam)
{
	printf("%S %S\n", pEntry32->szExePath, pEntry32->szModule);
	return true;
}

BOOL FindProcessCallback(PPROCESSENTRY32 pEntry32, PVOID pParam)
{
	TCHAR name[MAX_PATH] = { 0 };
	DWORD ret = CProcessUtils::GetProcessFileName(pEntry32->th32ProcessID, name);
	if (ret > 0)
		printf("%S\n", name);
	CProcessUtils::EnumProcessModule(pEntry32->th32ProcessID, EnumProcessModalCallback_, NULL);
	return TRUE;
}
*/
int _tmain(int argc, _TCHAR* argv[])
{
	CLdList<CLdString*> strings;
	strings.Append(new CLdString(L"abcd1"));
	strings.Append(new CLdString(L"abcd2"));
	strings.Append(new CLdString(L"abcd3"));
	strings.Append(new CLdString(L"abcd4"));
	strings.Append(new CLdString(L"abcd5"));
	strings.Append(new CLdString(L"abcd6"));
	strings.Append(new CLdString(L"abcd7"));
	strings.Append(new CLdString(L"abcd8"));
	strings.Append(new CLdString(L"abcd9"));

	CLdString* s;
	for (PListIndex index = strings.Begin(&s); index != NULL; index = strings.Behind(index, &s))
	{
		printf("%S   %d \n", *s, strings.GetCount());
	}

	printf("\n \n \n");

	strings.Remove(new CLdString(L"abcd4"));
	strings.Remove(new CLdString(L"abcd5"));
	strings.Remove(new CLdString(L"abcd6"));

	while(!strings.IsListEmpty())
	{
		printf("%S %d \n", *s, strings.GetCount());
		s = strings.Pop();
	}

	strings.Clear();

	printf("press any key exit");
	getchar();
	return 0;
}
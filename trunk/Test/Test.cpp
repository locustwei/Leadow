
#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include "LdLib.h"

/*
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	tstring path(_T("E:\\LdExplorerExtends\\trunk\\bin\\test.exe"));
	vector<ncFileHandle> vecHandles;
	if (!FindFileHandle(path.c_str(), vecHandles)) {
		return -1;
	}

	return (int) 0;
}
*/

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

int _tmain(int argc, _TCHAR* argv[])
{
	CProcessUtils::EnumProcess(FindProcessCallback, NULL);

	printf("press any key exit");
	getchar();
	return 0;
}

#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include <Commdlg.h>
#include <shellapi.h>
#include <io.h>
#include <fcntl.h>


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}


class CImpl: public IGernalCallback<PSH_HEAD_INFO>
{
public:
	BOOL GernalCallback_Callback(_SH_HEAD_INFO* pData, UINT_PTR Param) override
	{
		CLdArray<CLdString>* columes = (CLdArray<CLdString>*)Param;
		columes->Add(pData->szName);
		//printf("%S\n", pData->szName);

		return true;
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	CoInitialize(nullptr);

	CImpl impl;
	CLdArray<CLdString> columes;

	CSHFolders::EnumFolderColumes(L"C:\\", &impl, (UINT_PTR)&columes);
	CLdArray<TCHAR*> values;
	CSHFolders::GetFileAttributeValue(L"F:\\VHDtest.vhd", &values);

	for (int i = 0; i < values.GetCount(); i++)
		printf("%S === %S\n", columes[i].GetData(), values[i]);

	printf("\npress any key exit");
	getchar();
	return 0;
}

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


class CImpl: 
	public IGernalCallback<PSH_HEAD_INFO>
	,public ISortCompare<CLdString*>
{
public:
	CImpl()
	{
		Count = 0;
	};

	int Count;

	int ArraySortCompare(CLdString** it1, CLdString** it2) override
	{
		int k = _tcsicmp((*it1)->GetData(), (*it2)->GetData());
		if (k == 0)
			printf("%d\n", Count++);
		return k;
	};

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

	CLdArray<CLdString*> names;
	for (int i = 0; i < 100000; i++)
	{
		CLdString* str = new CLdString();
		CFileUtils::GenerateRandomFileName(20, str);
		names.Add(str);
	}

	CImpl impl;
	names.Sort(&impl);

	printf("\npress any key exit");
	getchar();
	return 0;
}

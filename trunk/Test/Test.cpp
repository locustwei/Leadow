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
#include <Ldlib.h>
#include "../LdFileSearch/SearchLibrary.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	CVolumeInfo volume;
	volume.SetFileName(L"E:\\");
	ISearchLibrary * library = 

	printf("\npress any key exit");
	getchar();
	return 0;
}

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

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

class CImp
{
public:
	CImp()
	{
		a = 10;
		b = 0;
	};
	int a;
	void p()
	{
		printf("cimp.a=%d\n", b);
	}

	void setb(int value)
	{
		b = value;
	}
protected:
	int b;
};

class C1: public CImp
{
public:
	void p()
	{
		printf("c1.a=%d\n", b);
	}

	void setb(int value)
	{
		b = value;
	}

};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	CFileInfo Info;
	Info.SetFileName(_T("C:\\Windows.old"));

	printf("%S \n", (TCHAR*)CDateTimeUtils::DateTimeToString(Info.GetLastAccessTime()));

	Info.FindFiles();


	printf("\npress any key exit");
	getchar();
	return 0;
}

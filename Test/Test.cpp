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

	FILE_BASIC_INFO m_Baseinfo;
	FILE_STANDARD_INFO m_StandrardInfo;

	WIN32_FILE_ATTRIBUTE_DATA fdata = {0};
	if (!GetFileAttributesEx(L"c:\\", GetFileExInfoStandard, &fdata))
		printf("error = %d", GetLastError());

	HANDLE hFile = CreateFile(L"G:\\Winobj.exe", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileInformationByHandleEx(hFile, FileStandardInfo, &m_StandrardInfo, sizeof(FILE_STANDARD_INFO));
		GetFileInformationByHandleEx(hFile, FileBasicInfo, &m_Baseinfo, sizeof(FILE_BASIC_INFO));
		CloseHandle(hFile);
	}

	printf("\npress any key exit");
	getchar();
	return 0;
}

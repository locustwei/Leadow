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

	CLdArray<TCHAR*> names;
	CFileUtils::GetFileADSNames(TEXT("G:\\TestFile"), &names);

	for(int i=0; i<names.GetCount(); i++)
	{
		HANDLE hFile = CreateFile(TEXT("G:\\TestFile:Stream"), // Filename
			GENERIC_WRITE,    // Desired access
			FILE_SHARE_WRITE, // Share flags
			NULL,             // Security Attributes
			OPEN_ALWAYS,      // Creation Disposition
			0,                // Flags and Attributes
			NULL);           // OVERLAPPED pointer
		DWORD nSize;
		nSize = GetFileSize(hFile, nullptr);
		CloseHandle(hFile);
		printf("%S %d\n", names[i], nSize);
		delete names[i];
	}

	/*
	HANDLE hFile, hStream;
	DWORD dwRet;
	for (int i = 0; i < name_array.GetCount(); i++)
	{
		printf("%S\n", name_array[i]);
	hFile = CreateFile(TEXT("G:\\TestFile"), // Filename
		GENERIC_WRITE,    // Desired access
		FILE_SHARE_WRITE, // Share flags
		NULL,             // Security Attributes
		OPEN_ALWAYS,      // Creation Disposition
		0,                // Flags and Attributes
		NULL);           // OVERLAPPED pointer
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("Cannot open TestFile\n");
	}
	else
	{
		WriteFile(hFile,              // Handle
			"This is TestFile", // Data to be written
			16,                 // Size of data, in bytes
			&dwRet,             // Number of bytes written
			NULL);             // OVERLAPPED pointer
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	hStream = CreateFile(TEXT("G:\\TestFile:Stream"), // Filename
		GENERIC_WRITE,           // Desired access
		FILE_SHARE_WRITE,        // Share flags
		NULL,                    // Security Attributes
		OPEN_ALWAYS,             // Creation Disposition
		0,                       // Flags and Attributes
		NULL);                  // OVERLAPPED pointer
	if (hStream == INVALID_HANDLE_VALUE)
		printf("Cannot open TestFile:Stream\n");
	else
	{
		WriteFile(hStream,                   // Handle
			"This is TestFile:Stream", // Data to be written
			23,                        // Size of data
			&dwRet,                    // Number of bytes written
			NULL);                     // OVERLAPPED pointer
		CloseHandle(hStream);
		hStream = INVALID_HANDLE_VALUE;
	}
*/

	printf("\npress any key exit");
	getchar();
	return 0;
}

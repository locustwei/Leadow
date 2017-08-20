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

	CDlgGetFileName dlg;
	dlg.SetOption(CDlgGetFileName::OPEN_FILE_OPTION | OFN_ALLOWMULTISELECT);
//	dlg.AddFilter(L"ddd", L"*.h;*.cpp");
//	dlg.AddFilter(L"bbbb", L"*.exe");
	dlg.SetInitDir(L"C:\\");
	dlg.OpenFile(nullptr, dft_folder);

	for (int i = 0; i < dlg.GetFileCount(); i++)
		printf("%S\n", dlg.GetFileName(i));

	printf("\npress any key exit");
	getchar();
	return 0;
}

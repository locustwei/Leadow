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

	CLdArray<TCHAR*> name_array;
	CFileUtils::GetFileADSNames(L"D:\\Ñ¸À×ÏÂÔØ\\Downloads\\UleadGIFAnimator.zip", &name_array);

	for (int i = 0; i < name_array.GetCount(); i++)
	{
		printf("%S\n", name_array[i]);

		//TCHAR* p1 = _tcsrchr(name_array[i], ':');
		//TCHAR* p2 = _tcsrchr(p1+1, ':');
		//TCHAR* p3 = _tcsrchr(p2 + 1, ':');
		//if (p2 - p1)
			//printf("%S,  %d \n", p1,0 );
		delete name_array[i];
	}

	printf("\npress any key exit");
	getchar();
	return 0;
}

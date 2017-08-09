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
#include "../LdFileSearch/SearchLibrary.h"
#include "../LdFileEraser/Erasure.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
	return (int) 0;
}

typedef PVOID(*Library_Init)(PAuthorization);

ISearchLibrary* InitLib(TCHAR * pLibFile, PAuthorization pAut)
{
	HMODULE hModule = LoadLibrary(pLibFile);
	if (hModule == NULL)
		return nullptr;
	Library_Init fnInit = (Library_Init)GetProcAddress(hModule, "API_Init");
	if (fnInit == NULL)
		return NULL;
	return (ISearchLibrary*)fnInit(pAut);
}

class CMftReadImpl: public IMftReadeHolder
{
public:
	CMftReadImpl()
	{
		nCount = 0;
	};

	BOOL EnumMftFileCallback(UINT64 ReferenceNumber, PFILE_INFO pFileInfo, UINT_PTR Param) override
	{
		
		printf("%lld  %S\n", nCount++);
		return true;
	};
private:
	UINT64 nCount;
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");

	DWORD cb;
	UCHAR buffer[1024];

	CVolumeInfo volume;
	volume.SetFileName(L"i:\\");
	
	CErasure erasure;
	ERASE_VOLUME_ANALY analy = { 0 };
	erasure.AnalysisVolume(&volume, &analy);

	printf("unusespace time=%lld  erase mft time=%lld clear time=%lld \n",
		(analy.freesize / (1024 * 1024 * 1024) * analy.writespeed) / 1000,
		(analy.trackCount / 100 * analy.cratespeed) / 1000,
		(analy.trackCount / 100 * analy.deletespeed) / 1000);

	printf("\npress any key exit");
	getchar();
	return 0;
}

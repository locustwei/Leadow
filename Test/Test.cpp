#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include "UIlib.h"
#include "LdLib.h"
#include "FileEraser.h"
#include <time.h>

/*
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
		return FALSE;
	DWORD ids[] = {12848, 6552, 3160, 6400};
	CProcessListView::KillProcess(NULL, ids, ARRAYSIZE(ids));

	::PostQuitMessage(0L);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();

	return (int) 0;
}

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
*/
class CEraseCallback : public IErasureCallback
{
public:
	CEraseCallback()
	{
	};
	~CEraseCallback()
	{
	};


	virtual BOOL ErasureStart(UINT nStepCount) override
	{
		printf("ErasureStart nStepCount = %d \n", nStepCount);
		return TRUE;
	}


	virtual BOOL ErasureCompleted(UINT nStep, DWORD dwErroCode) override
	{
		printf("ErasureCompleted nStep = %d dwErroCode = %d \n", nStep, dwErroCode);
		return TRUE;
	}


	virtual BOOL ErasureProgress(UINT nStep, UINT64 nMaxCount, UINT64 nCurent) override
	{
		//printf("ErasureProgress nStep = %d nMaxCount = %lld nCurent = %lld \n", nStep, nMaxCount, nCurent);
		return TRUE;
	}

private:

};

DWORD ResetFileDate(HANDLE hFile)
{
	FILE_BASIC_INFO binfo = { 0 };
	SYSTEMTIME ts = { 0 };

	ts.wYear = 1601;
	ts.wMonth = 1;
	ts.wDay = 1;
	ts.wMinute = 1;
	FILETIME ft;
	SystemTimeToFileTime(&ts, &ft);

	binfo.ChangeTime.HighPart = ft.dwHighDateTime;
	binfo.ChangeTime.LowPart= ft.dwLowDateTime;

	binfo.LastAccessTime.HighPart = ft.dwHighDateTime;
	binfo.LastAccessTime.LowPart = ft.dwLowDateTime;

	binfo.LastWriteTime.HighPart = ft.dwHighDateTime;
	binfo.LastWriteTime.LowPart = ft.dwLowDateTime;

	binfo.CreationTime.HighPart = ft.dwHighDateTime;
	binfo.CreationTime.LowPart = ft.dwLowDateTime;

	binfo.FileAttributes = FILE_ATTRIBUTE_SYSTEM;

	//return NtSetInformationFile(hFile, &binfo, sizeof(FILE_BASIC_INFO), FileBasicInformation);
	if (!SetFileInformationByHandle(hFile, FileBasicInfo, &binfo, sizeof(FILE_BASIC_INFO)))
		return GetLastError();
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	CEraseCallback callback;
	CErasure erasure;
	CErasureMethod method = CErasureMethod::Pseudorandom();
	erasure.UnuseSpaceErasure(CLdString(_T("F:")),method, &callback);

	printf("press any key exit");
	getchar();
	return 0;
}
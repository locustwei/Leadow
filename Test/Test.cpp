#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "UIlib.h"
#include "LdLib.h"
#include <time.h>
#include <Commdlg.h>
#include <shellapi.h>


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
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
/*
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
*/
class CHandleImp: public IEnumSystemHandleCallback
{
public:
	CHandleImp() {};
	~CHandleImp() {};


	virtual BOOL SystemHandleCallback(PSYSTEM_HANDLE pHandle, PVOID pParam) override
	{
		printf("SystemHandleCallback\n");
		return TRUE;
	}

private:

};
class CApp: public IGernalCallback<LPVOID>
{
public:
	BOOL GernalCallback_Callback(void* pData, UINT_PTR Param) override
	{
		printf("GernalCallback_Callback");
		return true;
	};
};
class CImp: public IThreadRunable
{
public:
	void ThreadRun(CThread* Sender, UINT_PTR Param) override
	{
		CLdApp::Send2MainThread((IGernalCallback<LPVOID>*)Param, 0);
	};
	void OnThreadInit(CThread* Sender, UINT_PTR Param) override
	{
	};
	void OnThreadTerminated(CThread* Sender, UINT_PTR Param) override
	{
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	CLdApp::Initialize(0);

	setlocale(LC_ALL, "chs");
	CApp app;
	CThread thread(new CImp());
	thread.Start((UINT_PTR)&app);
	printf("\npress any key exit");
	getchar();
	return 0;
}

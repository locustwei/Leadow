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




class CColCallbackImp: public IGernalCallback<PSH_HEAD_INFO>
{
public:
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override
	{
		if(pData->szName[0]==0x200E)
			printf("%d, %d, %S\n", pData->cxChar, pData->fmt, pData->szName + 1);
		else
			printf("%d, %d, %S\n", pData->cxChar, pData->fmt, pData->szName);
		return true;
	};
};

class CCallbackImp : public IGernalCallback<CLdArray<TCHAR*>*>
{
public:
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override
	{
		printf("\n");
		SHFILEINFO fi;
		SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
		printf("%S  ", fi.szDisplayName);
		for (int i = 1; i < pData->GetCount(); i++)
		{
				printf("%S  ", pData->Get(i));
		}
		return true;
	};
};

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, "chs");
	//CSHFolders::EnumFolderColumes(CSIDL_BITBUCKET, nullptr, new CColCallbackImp, 0);
	CSHFolders::EnumFolderObjects(CSIDL_BITBUCKET, nullptr, new CCallbackImp, 0);
	printf("\npress any key exit");
	getchar();
	return 0;
}

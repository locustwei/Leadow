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


class CImp: 
	public IGernalCallback<TCHAR*>, 
	public IGernalCallback<LPWIN32_FIND_DATA>,
	public IGernalCallback<PSH_HEAD_INFO>      //回收站文件显示列信息
{
public:
	BOOL GernalCallback_Callback(_WIN32_FIND_DATAW* pData, UINT_PTR Param) override
	{
		//if ((pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0)
		{
			TCHAR name[260] = { 0 };
			wsprintf(name, L"%s%s", (TCHAR*)Param, pData->cFileName);

			SHFILEOPSTRUCT fo;
			fo.pFrom = name;
			fo.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_NO_UI;
			fo.wFunc = FO_DELETE;
			int hr = SHFileOperation(&fo);
			if(hr!=0)
				printf("%S = %d\n", name, hr);
		}
		printf("%S%S\n", (TCHAR*)Param, pData->cFileName);
		return true;
	};

	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override
	{
		PSH_HEAD_INFO p = new SH_HEAD_INFO;
		p->cxChar = pData->cxChar;
		p->fmt = pData->fmt;
		p->szName = new TCHAR[_tcslen(pData->szName) + 1];
		_tcscpy(p->szName, pData->szName);
		printf("%S\n", pData->szName);;
		return true;
	};

	BOOL GernalCallback_Callback(wchar_t* pData, UINT_PTR Param) override
	{
		CLdArray<CLdString*>* Volumes = (CLdArray<CLdString*>*)Param;
		Volumes->Add(new CLdString(pData));
		return true;
	};
};

void EnumRecyleFiels()
{
	DWORD oldMode;
	CLdArray<CLdString*> Volumes;
	CLdString sid;
	CLdString recyclePath;

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);
	WIN_OS_TYPE os = GetOsType();
	if (GetCurrentUserSID(sid) != 0)
		return;
	CImp imp;
	CVolumeUtils::MountedVolumes(&imp, (UINT_PTR)&Volumes);

	for (int i = 0; i<Volumes.GetCount(); i++)
	{
		recyclePath = Volumes[i]->GetData();
		if (os >= Windows_Vista)
			recyclePath += _T("$RECYCLE.BIN");
		else
			recyclePath += _T("RECYCLE");

		switch (CVolumeUtils::GetVolumeFileSystem(Volumes[i]->GetData()))
		{
		case FS_NTFS:
			if (os < Windows_Vista)
				recyclePath += _T("R");
			//recyclePath += '\\';
			//recyclePath += sid;
			break;
		default:
			if (os < Windows_Vista)
				recyclePath += _T("D");
		}

		recyclePath += _T("\\");

		CFileUtils::FindFile(recyclePath, L"*", &imp, (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}

	SetThreadErrorMode(oldMode, &oldMode);
}

int _tmain(int argc, _TCHAR* argv[])
{
	CLdApp::Initialize(0);

	setlocale(LC_ALL, "chs");

//	CImp imp;
//	CSHFolders::EnumFolderColumes(CSIDL_DESKTOP, &imp, 0);
//	LPITEMIDLIST pidl;

	CLdArray<TCHAR*> values;
	HRESULT hr = CSHFolders::GetFileAttributeValue(L"C:\\swapfile.sys", &values);

	for(int i=0;i<values.GetCount();i++)
	{
		printf("%S\n", values[i]);
		delete values[i];
	}

	printf("\npress any key exit");
	getchar();
	return 0;
}

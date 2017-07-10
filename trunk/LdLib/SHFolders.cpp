// RecycleBinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SHFolders.h"
#include <Shlwapi.h>
#include <cstdio>

#pragma comment(lib,"Shlwapi.lib")

CLdString CSHFolders::GetKnownFolderPath(const REFKNOWNFOLDERID rfid, DWORD dwFlag, HANDLE hToken)
{
	TCHAR* result = nullptr;

	if (SHGetKnownFolderPath(rfid, dwFlag, hToken, &result) != S_OK)
		return _T("");
	else
	{
		CLdString r = result;
		CoTaskMemFree(result);
		return r;
	}
}
/*
HRESULT CSHFolders::GetFileAttributeValue(TCHAR * lpFullName, CLdArray<TCHAR*>* values)
{
	IShellFolder2* pDesktopFolder = nullptr;
	ULONG         chEaten;
	HRESULT       hr;
	LPITEMIDLIST pidl = nullptr;

	hr = GetShellFolder(CSIDL_DESKTOP, &pDesktopFolder);
	if (SUCCEEDED(hr))
	{
		hr = pDesktopFolder->ParseDisplayName(NULL, NULL, lpFullName, &chEaten, &pidl, nullptr);
		if(SUCCEEDED(hr))
		{
			int iSubItem = 0;
			SHELLDETAILS sd = { 0 };
			while (SUCCEEDED(pDesktopFolder->GetDetailsOf(pidl, iSubItem, &sd)))
			{
				TCHAR* szTemp = nullptr;
				StrRetToStr(&sd.str, pidl, &szTemp);  
				TCHAR* sz = new TCHAR[_tcslen(szTemp) + 1];
				_tcscpy(sz, szTemp);
				values->Add(sz);
				CoTaskMemFree(szTemp);
				iSubItem++;
			}
			CoTaskMemFree(pidl);
		}
		pDesktopFolder->Release();
	}
	return hr;
}
*/
HRESULT CSHFolders::GetShellFolder(DWORD dwFolder, IShellFolder2** pFolder)
{
	HRESULT			hr = S_OK;
	LPSHELLFOLDER	pDesktop = NULL;
	LPITEMIDLIST	pidl = NULL;

	hr = SHGetDesktopFolder(&pDesktop);
	if (SUCCEEDED(hr))
	{
		hr = SHGetSpecialFolderLocation(nullptr, dwFolder, &pidl);
		if (SUCCEEDED(hr))
		{
			if (dwFolder != CSIDL_DESKTOP)
				hr = pDesktop->BindToObject(pidl, nullptr, IID_IShellFolder2, (LPVOID *)pFolder);
			else
				hr = pDesktop->QueryInterface(IID_IShellFolder2, (LPVOID *)pFolder);
		}
	}
	if(pidl)
		CoTaskMemFree(pidl);
	if (pDesktop)
		pDesktop->Release();
	
	return hr;
}

HRESULT CSHFolders::EnumFolderObjects(DWORD dwFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
{
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPENUMIDLIST    penumFiles = nullptr;
	LPITEMIDLIST	pidl = NULL;
	CLdArray<TCHAR*> attributeValues;
	TCHAR del[] = { 0x200E, 0 };

	hr = GetShellFolder(dwFolder, &pFolder);

	if (SUCCEEDED(hr))
		hr = pFolder->EnumObjects(hOwnerWnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &penumFiles);

	if (SUCCEEDED(hr))
	{
		while (hr = penumFiles->Next(1, &pidl, NULL) != S_FALSE)
		{
			attributeValues.Add((TCHAR*)pidl);
			int iSubItem = 0;
			SHELLDETAILS sd = { 0 };
			while (SUCCEEDED(pFolder->GetDetailsOf(pidl, iSubItem, &sd) ))
			{
				TCHAR* szTemp = nullptr;
				//hr = pFolder->GetDetailsOf(pidl, iSubItem, &sd);
				StrRetToStr(&sd.str, pidl, &szTemp);  //第一个字符=0x200E表示靠右排列。
				TCHAR* sz = new TCHAR[_tcslen(szTemp) +1];
				_tcscpy(sz, szTemp);
				attributeValues.Add(sz);
				CoTaskMemFree(szTemp);
				iSubItem++;
			}

			if (!callback->GernalCallback_Callback(&attributeValues, Param))
			{
				CoTaskMemFree(pidl);
				break;
			}
			CoTaskMemFree(pidl);

			for (int i = 1; i<attributeValues.GetCount(); i++)
			{
				delete attributeValues[i];
			}
			attributeValues.Clear();

		}
	}

	for (int i = 1; i<attributeValues.GetCount(); i++)
	{
		delete attributeValues[i];
	}
	attributeValues.Clear();

	if (pFolder)
		pFolder->Release();

	return hr;
}

HRESULT CSHFolders::EnumFolderColumes(DWORD dwFolder, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd)
{
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPITEMIDLIST	pidl = NULL;
	SH_HEAD_INFO    colInfo;

	hr = GetShellFolder(dwFolder, &pFolder);
	if(SUCCEEDED(hr))
		hr = SHGetSpecialFolderLocation(nullptr, dwFolder, &pidl);
	int iSubItem = 0;
	while (SUCCEEDED(hr))
	{
		SHELLDETAILS sd = { 0 };
		HRESULT hr1 = pFolder->GetDetailsOf(NULL, iSubItem, &sd);
		if (SUCCEEDED(hr1))
		{
			colInfo.cxChar = sd.cxChar;
			colInfo.fmt = sd.fmt;
			StrRetToStr(&sd.str, pidl, &colInfo.szName);
			if (!callback->GernalCallback_Callback(&colInfo, Param))
			{
				hr = S_FALSE;
				break;
			}
			CoTaskMemFree(colInfo.szName);
		}
		else
			break;
		iSubItem++;
	}
	if (pFolder)
		pFolder->Release();
	return hr;
}


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

HRESULT CSHFolders::GetFileAttributeValue(TCHAR * lpFullName, CLdArray<TCHAR*>* values)
{
	IShellFolder2* pFolder = nullptr;
	HRESULT       hr;
	LPITEMIDLIST pidl = nullptr;

	PCUITEMID_CHILD pidl2 = nullptr;

	hr = GetFileItemid(lpFullName, &pidl);
	if (SUCCEEDED(hr))
	{
		hr = SHBindToParent(pidl, IID_IShellFolder2, (void**)&pFolder, &pidl2);
	}
	if (SUCCEEDED(hr))
	{
		int iSubItem = 0;
		SHELLDETAILS sd = { 0 };
		while (SUCCEEDED(pFolder->GetDetailsOf(pidl2, iSubItem, &sd)))
		{
			
			iSubItem++;
			if (sd.cxChar == 0)
				continue;
			TCHAR* szTemp = nullptr;
			StrRetToStr(&sd.str, pidl2, &szTemp);
			TCHAR* sz = new TCHAR[_tcslen(szTemp) + 1];
			_tcscpy(sz, szTemp);
			values->Add(sz);
			CoTaskMemFree(szTemp);
		}
		CoTaskMemFree(pidl);
		pFolder->Release();
	}
	return hr;
}

HRESULT CSHFolders::GetShellFolder(DWORD dwFolder, IShellFolder2** pFolder, LPITEMIDLIST* pidlist)
{
	HRESULT			hr = S_OK;
	LPSHELLFOLDER	pDesktop = NULL;
	LPITEMIDLIST    pidl = nullptr;

	hr = SHGetDesktopFolder(&pDesktop);
	if (SUCCEEDED(hr))
	{
		hr = SHGetSpecialFolderLocation(nullptr, dwFolder, &pidl);
		if (SUCCEEDED(hr))
		{
			if (pidlist)
				*pidlist = pidl;
			if (dwFolder != CSIDL_DESKTOP)
				hr = pDesktop->BindToObject(pidl, nullptr, IID_IShellFolder2, (LPVOID *)pFolder);
			else
				hr = pDesktop->QueryInterface(IID_IShellFolder2, (LPVOID *)pFolder);
		}
	}
	if (!pidlist && pidl)
		CoTaskMemFree(pidl);
	if (pDesktop)
		pDesktop->Release();
	
	return hr;
}

HRESULT CSHFolders::GetShellFolder(TCHAR * lpFullName, IShellFolder2 ** pFolder, LPITEMIDLIST* pidlist)
{
	HRESULT       hr;
	LPITEMIDLIST pidl = nullptr;
	//PCUITEMID_CHILD pidl2 = nullptr;

	hr = GetFileItemid(lpFullName, &pidl);
	if (SUCCEEDED(hr))
	{
		if (pidlist)
			*pidlist = pidl;
		hr = SHBindToObject(nullptr, pidl, nullptr, IID_IShellFolder2, (void**)pFolder);

		if(!pidlist)
			CoTaskMemFree(pidl);
	}
	return hr;
}

HRESULT CSHFolders::GetFileItemid(TCHAR * lpFullName, LPITEMIDLIST * pidl)
{
	IShellFolder2* pDesktopFolder = nullptr;
	ULONG         chEaten;
	HRESULT       hr;

	hr = GetShellFolder((DWORD)CSIDL_DESKTOP, &pDesktopFolder, nullptr);
	if (SUCCEEDED(hr))
	{
		hr = pDesktopFolder->ParseDisplayName(NULL, NULL, lpFullName, &chEaten, pidl, nullptr);
	}
	return hr;
}

HRESULT CSHFolders::EnumFolderColumes(IShellFolder2 * pFolder, LPITEMIDLIST pidl, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param)
{
	HRESULT			hr = S_OK;
	int             iSubItem = 0;
	SH_HEAD_INFO    colInfo;
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
	return hr;
}

HRESULT CSHFolders::EnumFolderObjects(DWORD dwFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
{
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPENUMIDLIST    penumFiles = nullptr;
	LPITEMIDLIST	pidl = NULL;
	CLdArray<TCHAR*> attributeValues;

	hr = GetShellFolder(dwFolder, &pFolder, nullptr);

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

HRESULT CSHFolders::EnumFolderObjects(TCHAR* lpFullName, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
{
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPENUMIDLIST    penumFiles = nullptr;
	LPITEMIDLIST	pidl = NULL;
	CLdArray<TCHAR*> attributeValues; 	
	
//	LPITEMIDLIST fpidl = nullptr;
//	PCUITEMID_CHILD pidl2 = nullptr;

	hr = GetShellFolder(lpFullName, &pFolder, nullptr);

	if (SUCCEEDED(hr))
		hr = pFolder->EnumObjects(hOwnerWnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &penumFiles);

	if (SUCCEEDED(hr))
	{

		while (hr = penumFiles->Next(1, &pidl, NULL) != S_FALSE)
		{
			attributeValues.Add((TCHAR*)pidl);
			int iSubItem = 0;
			SHELLDETAILS sd = { 0 };
			while (SUCCEEDED(pFolder->GetDetailsOf(pidl, iSubItem, &sd)))
			{
				
				TCHAR* szTemp = nullptr;
				//hr = pFolder->GetDetailsOf(pidl, iSubItem, &sd);
				StrRetToStr(&sd.str, pidl, &szTemp);  //第一个字符=0x200E表示靠右排列。
				TCHAR* sz = new TCHAR[_tcslen(szTemp) + 1];
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

	hr = GetShellFolder(dwFolder, &pFolder, &pidl);
	hr = EnumFolderColumes(pFolder, pidl, callback, Param);
	
	if (pFolder)
		pFolder->Release();
	return hr;
}

HRESULT CSHFolders::EnumFolderColumes(TCHAR * lpFullName, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd)
{
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	SH_HEAD_INFO    colInfo;

	hr = GetShellFolder(lpFullName, &pFolder, nullptr);
	int iSubItem = 0;
	while (SUCCEEDED(hr))
	{
		SHELLDETAILS sd = { 0 };
		HRESULT hr1 = pFolder->GetDetailsOf(NULL, iSubItem, &sd);
		if (SUCCEEDED(hr1))
		{
			colInfo.cxChar = sd.cxChar;
			colInfo.fmt = sd.fmt;
			StrRetToStr(&sd.str, nullptr, &colInfo.szName);
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


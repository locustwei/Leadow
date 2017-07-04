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

HRESULT CSHFolders::EnumFolderObjects(DWORD dwFolder, HWND hOwnerWnd, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param)
{
	LPSHELLFOLDER	pDesktop = NULL;
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPENUMIDLIST    penumFiles = nullptr;
	LPITEMIDLIST	pidl = NULL;
	SHFILEINFO		fi;
	CLdArray<TCHAR*> attributeValues;
	TCHAR del[] = { 0x200E, 0 };

	hr = SHGetDesktopFolder(&pDesktop);
	if (SUCCEEDED(hr))
	{
//		if (dwFolder != CSIDL_DESKTOP)
//		{
		hr = SHGetSpecialFolderLocation(hOwnerWnd, dwFolder, &pidl);
		if (SUCCEEDED(hr))
		{
			hr = pDesktop->BindToObject(pidl, nullptr, IID_IShellFolder2, (LPVOID *)&pFolder);
			CoTaskMemFree(pidl);
		}
	}

	if (SUCCEEDED(hr))
	{
		pFolder->EnumObjects(hOwnerWnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &penumFiles);
		if (SUCCEEDED(hr))
		{
			while (hr = penumFiles->Next(1, &pidl, NULL) != S_FALSE)
			{

//				ZeroMemory(&fi, sizeof(fi));
//				hr = SHGetFileInfo((LPCTSTR)pidl, 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
//				if (SUCCEEDED(hr))
//				{
//					attributeValues.Add((TCHAR*)pidl);
//				}
				attributeValues.Add((TCHAR*)pidl);
				int iSubItem = 0;
				while (SUCCEEDED(hr))
				{
					SHELLDETAILS sd = { 0 };
					TCHAR* szTemp = nullptr;
					hr = pFolder->GetDetailsOf(pidl, iSubItem, &sd);
					if (SUCCEEDED(hr))
					{
						StrRetToStr(&sd.str, pidl, &szTemp);  //第一个字符=0x200E表示靠右排列。
						TCHAR* sz = new TCHAR[_tcslen(szTemp) +1];
						int k = 0;
						for(int i=0; i<_tcslen(szTemp); i++)
						{
							//if(szTemp[i]!=0x200E)
							{
								sz[k++] = szTemp[i];
							}
						}
						sz[k++] = '\0';
						attributeValues.Add(sz);
						CoTaskMemFree(szTemp);
					}
					iSubItem++;
				}

				if (!callback->GernalCallback_Callback(&attributeValues, Param))
					hr = S_FALSE;

				CoTaskMemFree(pidl);
				for (int i = 1; i<attributeValues.GetCount(); i++)
				{
					delete attributeValues[i];
				}
				attributeValues.Clear();
			}
		}
	}

	if (pDesktop)
		pDesktop->Release();
	if (pFolder)
		pFolder->Release();

	return hr;
}

HRESULT CSHFolders::EnumFolderColumes(DWORD dwFolder, HWND hOwnerWnd, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param)
{
	LPSHELLFOLDER	pDesktop = NULL;
	HRESULT			hr = S_OK;
	IShellFolder2*	pFolder = NULL;
	LPITEMIDLIST	pidl = NULL;
	SH_HEAD_INFO    colInfo;

	hr = SHGetDesktopFolder(&pDesktop);
	if (SUCCEEDED(hr))
	{
//		if (dwFolder != CSIDL_DESKTOP)
//		{
		hr = SHGetSpecialFolderLocation(hOwnerWnd, dwFolder, &pidl);
		if (SUCCEEDED(hr))
		{
			hr = pDesktop->BindToObject(pidl, nullptr, IID_IShellFolder2, (LPVOID *)&pFolder);
			int iSubItem = 0;
			while (SUCCEEDED(hr))
			{
				SHELLDETAILS sd = { 0 };
				hr = pFolder->GetDetailsOf(NULL, iSubItem, &sd);
				if (SUCCEEDED(hr))
				{
					colInfo.cxChar = sd.cxChar;
					colInfo.fmt = sd.fmt;
					StrRetToStr(&sd.str, pidl, &colInfo.szName);
					callback->GernalCallback_Callback(&colInfo, Param);
					CoTaskMemFree(colInfo.szName);
				}
				iSubItem++;
			}
			hr = S_OK;
			CoTaskMemFree(pidl);
		}
	}
	return hr;
}


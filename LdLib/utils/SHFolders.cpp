// RecycleBinDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SHFolders.h"
#include <Shlwapi.h>
#include <propkey.h>

#pragma comment(lib,"Shlwapi.lib")

namespace LeadowLib {
	CLdString CSHFolders::GetKnownFolderPath(const REFKNOWNFOLDERID rfid, DWORD dwFlag, HANDLE hToken)
	{
		wchar_t* result = nullptr;

		if (SHGetKnownFolderPath(rfid, dwFlag, hToken, &result) != S_OK)
			return _T("");
		else
		{
			CLdString r = (wchar_t*)result;
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
		if (!lpFullName)
			return (HRESULT)-1;

		TCHAR* lpName = _tcsrchr(lpFullName, '\\');
		if (lpName == nullptr || _tcslen(lpName) == 1)
		{
			hr = GetFileItemid(lpFullName, &pidl);
			hr = SHBindToParent(pidl, IID_IShellFolder2, (void**)&pFolder, &pidl2);
		}
		else
		{
			CLdString str = lpFullName;
			str.Delete(lpName - lpFullName, 260);
			hr = GetShellFolder(str, (IShellFolder2**)&pFolder, nullptr);
			ULONG eaten;
			hr = pFolder->ParseDisplayName(nullptr, nullptr, ++lpName, &eaten, &pidl, nullptr);
			pidl2 = pidl;
		}

		if (SUCCEEDED(hr))
		{
			//values->Add((TCHAR*)pidl);
			int iSubItem = 0;
			SHELLDETAILS sd = { 0 };
			while (SUCCEEDED(pFolder->GetDetailsOf(pidl2, iSubItem, &sd)))
			{
				SHCOLSTATEF cstat;
				pFolder->GetDefaultColumnState(iSubItem, &cstat);

				iSubItem++;
				if ((cstat & SHCOLSTATE_SECONDARYUI) != 0)
					break;

				TCHAR* szTemp = nullptr;
				StrRetToStr(&sd.str, pidl, &szTemp);
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
		HRESULT			hr;
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
		LPSHELLFOLDER	pDesktop = NULL;
		//PCUITEMID_CHILD pidl2 = nullptr;

		hr = GetFileItemid(lpFullName, &pidl);
		if (SUCCEEDED(hr))
		{
			if (pidlist)
				*pidlist = pidl;
			//hr = SHBindToObject(nullptr, pidl, nullptr, IID_IShellFolder2, (void**)pFolder);
			hr = SHGetDesktopFolder(&pDesktop);
			if (SUCCEEDED(hr))
			{
				hr = pDesktop->BindToObject(pidl, nullptr, IID_IShellFolder2, (LPVOID *)pFolder);
			}

			if (pDesktop)
				pDesktop->Release();
			if (!pidlist)
				CoTaskMemFree(pidl);
		}
		return hr;
	}

	HRESULT CSHFolders::GetFileItemid(TCHAR * lpFullName, LPITEMIDLIST * pidl)
	{
		return SHParseDisplayName(lpFullName, nullptr, pidl, 0, nullptr);

		//	IShellFolder2* pDesktopFolder = nullptr;
		//	ULONG         chEaten;
		//	HRESULT       hr;
		//
		//	hr = GetShellFolder((DWORD)CSIDL_DESKTOP, &pDesktopFolder, nullptr);
		//	if (SUCCEEDED(hr))
		//	{
		//		hr = pDesktopFolder->ParseDisplayName(NULL, NULL, lpFullName, &chEaten, pidl, nullptr);
		//	}
		//	return hr;
	}

	HRESULT CSHFolders::EnumFolderColumes(IShellFolder2 * pFolder, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param)
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
				StrRetToStr(&sd.str, nullptr, &colInfo.szName);
				//printf("%d    ", iSubItem);
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

	HRESULT CSHFolders::EnumFolderObjects(IShellFolder2 * pFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
	{
		HRESULT			hr;
		LPENUMIDLIST    penumFiles = nullptr;
		LPITEMIDLIST	pidl = NULL;
		CLdArray<TCHAR*> attributeValues;

		hr = pFolder->EnumObjects(hOwnerWnd, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS | SHCONTF_INCLUDEHIDDEN, &penumFiles);

		if (SUCCEEDED(hr))
		{
			while (penumFiles->Next(1, &pidl, NULL) != S_FALSE)
			{
				attributeValues.Add((TCHAR*)pidl);
				int iSubItem = 0;
				SHELLDETAILS sd = { 0 };
				while (SUCCEEDED(pFolder->GetDetailsOf(pidl, iSubItem, &sd)))
				{
					TCHAR* szTemp = nullptr;
					SHCOLSTATEF cstat;
					pFolder->GetDefaultColumnState(iSubItem, &cstat);

					iSubItem++;
					if ((cstat & SHCOLSTATE_SECONDARYUI) != 0)
						break;
					//printf("%d\n", iSubItem);

					StrRetToStr(&sd.str, pidl, &szTemp);
					TCHAR* sz = new TCHAR[_tcslen(szTemp) + 1];
					_tcscpy(sz, szTemp);
					attributeValues.Add(sz);
					CoTaskMemFree(szTemp);
				}

				BOOL b = callback->GernalCallback_Callback(&attributeValues, Param);
				CoTaskMemFree(pidl);

				for (int i = 1; i < attributeValues.GetCount(); i++)
				{
					delete attributeValues[i];
				}
				attributeValues.Clear();
				if (!b)
					break;
			}
		}

		for (int i = 1; i < attributeValues.GetCount(); i++)
		{
			delete attributeValues[i];
		}
		attributeValues.Clear();

		if (pFolder)
			pFolder->Release();

		return hr;

	}

	HRESULT CSHFolders::EnumFolderObjects(DWORD dwFolder, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
	{
		HRESULT			hr;
		IShellFolder2*	pFolder = NULL;

		hr = GetShellFolder(dwFolder, &pFolder, nullptr);
		if (SUCCEEDED(hr))
			return EnumFolderObjects(pFolder, callback, Param, hOwnerWnd);
		else
			return hr;
	}

	HRESULT CSHFolders::EnumFolderObjects(TCHAR* lpFullName, IGernalCallback<CLdArray<TCHAR*>*>* callback, UINT_PTR Param, HWND hOwnerWnd)
	{
		HRESULT			hr;
		IShellFolder2*	pFolder = NULL;

		hr = GetShellFolder(lpFullName, &pFolder, nullptr);

		if (SUCCEEDED(hr))
			return EnumFolderObjects(pFolder, callback, Param, hOwnerWnd);
		else
			return hr;
	}

	HRESULT CSHFolders::EnumFolderColumes(DWORD dwFolder, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd)
	{
		HRESULT			hr;
		IShellFolder2*	pFolder = NULL;

		hr = GetShellFolder(dwFolder, &pFolder, nullptr);
		if (SUCCEEDED(hr))
			hr = EnumFolderColumes(pFolder, callback, Param);

		if (pFolder)
			pFolder->Release();
		return hr;
	}

	HRESULT CSHFolders::EnumFolderColumes(TCHAR * lpFullName, IGernalCallback<PSH_HEAD_INFO>* callback, UINT_PTR Param, HWND hOwnerWnd)
	{
		HRESULT			hr;
		IShellFolder2*	pFolder = NULL;

		hr = GetShellFolder(lpFullName, &pFolder, nullptr);
		if (SUCCEEDED(hr))
			hr = EnumFolderColumes(pFolder, callback, Param);

		if (pFolder)
			pFolder->Release();
		return hr;
	}

}
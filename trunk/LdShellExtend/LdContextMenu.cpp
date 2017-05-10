// LdContextMenu.cpp : CLdContextMenu 的实现

#include "stdafx.h"
#include "LdContextMenu.h"
#include <string.h>
#include "FileProtectPipeFlow.h"

#define LD_CONTEXT_MENU _T("&Leadow Context Menu")

// CLdContextMenu

HRESULT STDMETHODCALLTYPE CLdContextMenu::QueryContextMenu(__in HMENU hmenu, __in UINT indexMenu, __in UINT idCmdFirst, __in UINT idCmdLast, __in UINT uFlags)
{
	HMENU hSubmenu;
	UINT id = idCmdFirst;

	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	
	switch(m_ContextMenuType){
	case CONTEX_MENU_ALLFILE:
		hSubmenu = CreateAllFileMenum(id);
		break;
	case CONTEX_MENU_FOLDER:
		hSubmenu = CreateFolderMenum(id);
		break;
	case CONTEX_MENU_DRIVE: 
		hSubmenu = CreateDriveMenum(id);
		break;
	case CONTEX_MENU_DIR_BKG:
		hSubmenu = CreateDirbkgMenum(id);
		break;
	default:
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );
	}

	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = id++;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = LD_CONTEXT_MENU;

	InsertMenuItem ( hmenu, indexMenu, TRUE, &mii );
	
	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, id - idCmdFirst );

}

HRESULT STDMETHODCALLTYPE CLdContextMenu::InvokeCommand(__in CMINVOKECOMMANDINFO *pici)
{
	if ( 0 != HIWORD( pici->lpVerb ) )
		return E_INVALIDARG;

	switch(m_ContextMenuType){
	case CONTEX_MENU_ALLFILE:
		switch(LOWORD(pici->lpVerb)){
		case 0: //
			CFileProtectPipeFlow::StartPipeFlow(m_SelectCount, m_SelectFiles);
			break;
		case 1: //Force Delete
			break;
		case 2: //earse file
			break;
		}
		break;
	case CONTEX_MENU_FOLDER:
		break;
	case CONTEX_MENU_DRIVE: 
		break;
	case CONTEX_MENU_DIR_BKG:
		break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}
//显示提示信息
HRESULT STDMETHODCALLTYPE CLdContextMenu::GetCommandString(__in UINT_PTR idCmd, __in UINT uType, __reserved UINT *pReserved, __out_awcount(!(uType & GCS_UNICODE) , cchMax) LPSTR pszName, __in UINT cchMax)
{
	USES_CONVERSION;

	// Check idCmd, it must be 0 since we have only one menu item.
	if ( 0 != idCmd )
		return E_INVALIDARG;

	// If Explorer is asking for a help string, copy our string into the
	// supplied buffer.
	if ( uType & GCS_HELPTEXT )
	{
		LPCTSTR szText = _T("This is the simple shell extension's help");

		if ( uType & GCS_UNICODE )
		{
			// We need to cast pszName to a Unicode string, and then use the
			// Unicode string copy API.
			lstrcpynW ( (LPWSTR) pszName, T2CW(szText), cchMax );
		}
		else
		{
			// Use the ANSI string copy API to return the help string.
			lstrcpynA ( pszName, T2CA(szText), cchMax );
		}

		return S_OK;
	}

	return E_INVALIDARG;
}
/*
LPWSTR GetKeyPathFromKKEY(HKEY key)
{
	LPWSTR keyPath = NULL;
	if (key != NULL)
	{
		HMODULE dll = LoadLibrary(L"ntdll.dll");
		if (dll != NULL) {
			typedef DWORD (__stdcall *NtQueryKey)(
				HANDLE  KeyHandle,
				int KeyInformationClass,
				PVOID  KeyInformation,
				ULONG  Length,
				PULONG  ResultLength);

			NtQueryKey func = reinterpret_cast<NtQueryKey>(::GetProcAddress(dll, "NtQueryKey"));

			if (func != NULL) {
				DWORD size = 0;
				DWORD result = 0;
				result = func(key, 3, 0, 0, &size);
				if (result != 0)
				{
					size = size + 2;
					wchar_t* buffer = new wchar_t[size/sizeof(wchar_t)]; // size is in bytes
					if (buffer != NULL)
					{
						result = func(key, 3, buffer, size, &size);
						if (result == 0)
						{
							buffer[size / sizeof(wchar_t)] = L'\0';
							keyPath = buffer + 2;
						}
					}
				}
			}

			FreeLibrary(dll);
		}
	}
	return keyPath;
}
*/
HRESULT STDMETHODCALLTYPE CLdContextMenu::Initialize(__in_opt PCIDLIST_ABSOLUTE pidlFolder, __in_opt IDataObject *pDataObj, __in_opt HKEY hkeyProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	DWORD lRet, cb = sizeof(DWORD);
	HDROP hDrop;
	int uNumFiles;

	if(NULL == hkeyProgID)
		return E_INVALIDARG;
	lRet = RegGetValue(hkeyProgID, _T("shellex\\ContextMenuHandlers\\") REG_KEYNAME,  REG_TYPE_VALUE, RRF_RT_REG_DWORD, NULL, (LPBYTE)&m_ContextMenuType, &cb);
	if(lRet != ERROR_SUCCESS)
		return E_INVALIDARG;

	switch(m_ContextMenuType){
	case CONTEX_MENU_ALLFILE:
	case CONTEX_MENU_FOLDER:
	case CONTEX_MENU_DRIVE:
		if(!pDataObj)
			return E_INVALIDARG;
		if ( FAILED( pDataObj->GetData( &fmt, &stg )))
		{
			return E_INVALIDARG;
		}

		hDrop = (HDROP) GlobalLock ( stg.hGlobal );
		if ( NULL == hDrop )
			return E_INVALIDARG;

		uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );
		if ( 0 == uNumFiles )
		{
			GlobalUnlock ( stg.hGlobal );
			ReleaseStgMedium ( &stg );
			return E_INVALIDARG;
		}

		m_SelectFiles = new LPTSTR[uNumFiles];
		ZeroMemory(m_SelectFiles, uNumFiles * sizeof(LPTSTR));

		for(int i=0; i<uNumFiles; i++)
		{
			m_SelectFiles[i] = new TCHAR[MAX_PATH];
			ZeroMemory(m_SelectFiles[i], MAX_PATH * sizeof(TCHAR));
			if (!DragQueryFile ( hDrop, i, m_SelectFiles[i], MAX_PATH )){
				delete m_SelectFiles[i];
				m_SelectFiles[i] = NULL;
				break;
			}
		}
		m_SelectCount = uNumFiles;

		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );

		break;
	case CONTEX_MENU_DIR_BKG:
		if(!pidlFolder)
			return E_INVALIDARG;

		m_SelectCount = 1;
		m_SelectFiles = new LPTSTR[m_SelectCount];
		m_SelectFiles[0] = new TCHAR[MAX_PATH];
		ZeroMemory(m_SelectFiles[0], (MAX_PATH) * sizeof(WCHAR));

		if(!SHGetPathFromIDList(pidlFolder, m_SelectFiles[0] )){
			delete [] m_SelectFiles[0];
			delete [] m_SelectFiles;
			m_SelectFiles = NULL;
			m_SelectCount = 0;
			return E_INVALIDARG;
		}
		break;
	default:
		return E_INVALIDARG;
	}

	return S_OK;
}

//************************************
// Method:    CreateAllFileMenum
// FullName:  CLdContextMenu::CreateAllFileMenum
// Access:    private 
// Returns:   HMENU
// Qualifier: 所有文件（*）右键菜单
// Parameter: UINT & idCmdFirst
//************************************
HMENU CLdContextMenu::CreateAllFileMenum(UINT& idCmdFirst)
{
	HMENU hSubmenu = CreatePopupMenu();
	int pos = 0;

	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Hide") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION | MF_SEPARATOR, 0, _T("-") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&ForceDelete") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&SafeDelete") );
	if(m_SelectCount == 1)
	{
		InsertMenu( hSubmenu, pos++, MF_BYPOSITION | MF_SEPARATOR, 0, _T("-") );
		InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("Extend Property") );
	}
	return hSubmenu;
}

//************************************
// Method:    CreateFolderMenum
// FullName:  CLdContextMenu::CreateFolderMenum
// Access:    private 
// Returns:   HMENU
// Qualifier: 目录右键菜单
// Parameter: UINT & idCmdFirst
//************************************
HMENU CLdContextMenu::CreateFolderMenum(UINT& idCmdFirst)
{
	HMENU hSubmenu = CreatePopupMenu();
	int pos = 0;

	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Hide") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION | MF_SEPARATOR, 0, _T("-") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&ForceDelete") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&SafeDelete") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION | MF_SEPARATOR, 0, _T("-") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Seach File") );

	return hSubmenu;
}

//************************************
// Method:    CreateDriveMenum
// FullName:  CLdContextMenu::CreateDriveMenum
// Access:    private 
// Returns:   HMENU
// Qualifier: 磁盘驱动器右键菜单
// Parameter: UINT & idCmdFirst
//************************************
HMENU CLdContextMenu::CreateDriveMenum(UINT& idCmdFirst)
{
	HMENU hSubmenu = CreatePopupMenu();
	int pos = 0;

	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Disk Clean") );
	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Disk Erase") );

	return hSubmenu;
}

//************************************
// Method:    CreateDirbkgMenum
// FullName:  CLdContextMenu::CreateDirbkgMenum
// Access:    private 
// Returns:   HMENU
// Qualifier: 未选中文件（目录空白处）右键菜单
// Parameter: UINT & idCmdFirst
//************************************
HMENU CLdContextMenu::CreateDirbkgMenum(UINT& idCmdFirst)
{
	HMENU hSubmenu = CreatePopupMenu();
	int pos = 0;

	InsertMenu( hSubmenu, pos++, MF_BYPOSITION, idCmdFirst++, _T("&Seach File") );

	return hSubmenu;
}

BOOL RunInvoker(LD_FUNCTION_ID id, DWORD Flag, LPCTSTR lpPipeName)
{
	TCHAR FileName[MAX_PATH] = { 0 };
	if (!GetModuleFileName(0, FileName, MAX_PATH))
		return false;

	TCHAR* s = _tcsrchr(FileName, '\\');
	if (s == NULL)
		return false;
	s += 1;
	_tcscpy(s, _T("LdInvoker_d64.exe"));

	TCHAR Params[100] = { 0 };
	wsprintf(Params, _T("%d %d %s"), id, Flag, lpPipeName);
	return ShellExecute(NULL, NULL, FileName, Params, NULL, SW_SHOWNORMAL) != NULL;

}

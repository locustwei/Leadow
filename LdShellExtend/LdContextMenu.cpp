// LdContextMenu.cpp : CLdContextMenu 的实现

#include "stdafx.h"
#include "LdContextMenu.h"


// CLdContextMenu

HRESULT STDMETHODCALLTYPE CLdContextMenu::QueryContextMenu(__in HMENU hmenu, __in UINT indexMenu, __in UINT idCmdFirst, __in UINT idCmdLast, __in UINT uFlags)
{
	// If the flags include CMF_DEFAULTONLY then we shouldn't do anything.
	if ( uFlags & CMF_DEFAULTONLY )
		return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, 0 );

	// First, create and populate a submenu.
	HMENU hSubmenu = CreatePopupMenu();
	UINT uID = idCmdFirst;

	InsertMenu ( hSubmenu, 0, MF_BYPOSITION, uID++, _T("&Notepad") );
	InsertMenu ( hSubmenu, 1, MF_BYPOSITION, uID++, _T("&Internet Explorer") );

	// Insert the submenu into the ctx menu provided by Explorer.
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };

	mii.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID;
	mii.wID = uID++;
	mii.hSubMenu = hSubmenu;
	mii.dwTypeData = _T("C&P Open With");

	InsertMenuItem ( hmenu, indexMenu, TRUE, &mii );

	return MAKE_HRESULT ( SEVERITY_SUCCESS, FACILITY_NULL, uID - idCmdFirst );

}

HRESULT STDMETHODCALLTYPE CLdContextMenu::InvokeCommand(__in CMINVOKECOMMANDINFO *pici)
{
	// If lpVerb really points to a string, ignore this function call and bail out.
	if ( 0 != HIWORD( pici->lpVerb ) )
		return E_INVALIDARG;

	// Get the command index - the only valid one is 0.
	switch ( LOWORD( pici->lpVerb) )
	{
	case 0:
		{
			TCHAR szMsg [MAX_PATH + 32];

			wsprintf ( szMsg, _T("The selected file was:\n\n%s"), L"m_szFile" );

			MessageBox ( pici->hwnd, szMsg, _T("SimpleShlExt"),
				MB_ICONINFORMATION );

			return S_OK;
		}
		break;

	default:
		return E_INVALIDARG;
		break;
	}
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


HRESULT STDMETHODCALLTYPE CLdContextMenu::Initialize(__in_opt PCIDLIST_ABSOLUTE pidlFolder, __in_opt IDataObject *pDataObj, __in_opt HKEY hkeyProgID)
{
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stg = { TYMED_HGLOBAL };
	HDROP     hDrop;
	TCHAR m_szSelectedFile[MAX_PATH+2] = {0};

	if(!pidlFolder && !pDataObj)
		return E_INVALIDARG;

	if(!pDataObj){
		if(SHGetPathFromIDList(pidlFolder, m_szSelectedFile )){
			OutputDebugString(m_szSelectedFile);
			return S_OK;
		}else
			return E_INVALIDARG;
	}
	// Look for CF_HDROP data in the data object.
	if ( FAILED( pDataObj->GetData ( &fmt, &stg )))
	{
		// Nope! Return an "invalid argument" error back to Explorer.
		return E_INVALIDARG;
	}

	// Get a pointer to the actual data.
	hDrop = (HDROP) GlobalLock ( stg.hGlobal );

	// Make sure it worked.
	if ( NULL == hDrop )
		return E_INVALIDARG;

	// Sanity check - make sure there is at least one filename.
	UINT uNumFiles = DragQueryFile ( hDrop, 0xFFFFFFFF, NULL, 0 );

	if ( 0 == uNumFiles )
	{
		GlobalUnlock ( stg.hGlobal );
		ReleaseStgMedium ( &stg );
		return E_INVALIDARG;
	}

	HRESULT hr = S_OK;

	// Get the name of the first file and store it in our member variable m_szFile.
	if ( 0 == DragQueryFile ( hDrop, 0, m_szSelectedFile, MAX_PATH ))
		hr = E_INVALIDARG;
	else
	{
		// Quote the name if it contains spaces (needed so the cmd line is built properly)
		PathQuoteSpaces ( m_szSelectedFile );
		OutputDebugString(m_szSelectedFile);
	}

	GlobalUnlock ( stg.hGlobal );
	ReleaseStgMedium ( &stg );

	return hr;
}

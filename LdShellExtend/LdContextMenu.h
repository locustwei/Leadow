// LdContextMenu.h : CLdContextMenu µÄÉùÃ÷

#pragma once
#include "resource.h"       // Ö÷·ûºÅ
#include "LdShellExtend_i.h"

using namespace ATL;


// CLdContextMenu

class ATL_NO_VTABLE CLdContextMenu :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLdContextMenu, &CLSID_LdContextMenu>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CLdContextMenu()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LDCONTEXTMENU)


BEGIN_COM_MAP(CLdContextMenu)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	virtual HRESULT STDMETHODCALLTYPE Initialize(__in_opt PCIDLIST_ABSOLUTE pidlFolder, __in_opt IDataObject *pdtobj, __in_opt HKEY hkeyProgID);

	virtual HRESULT STDMETHODCALLTYPE QueryContextMenu(__in HMENU hmenu, __in UINT indexMenu, __in UINT idCmdFirst, __in UINT idCmdLast, __in UINT uFlags);

	virtual HRESULT STDMETHODCALLTYPE InvokeCommand(__in CMINVOKECOMMANDINFO *pici);

	virtual HRESULT STDMETHODCALLTYPE GetCommandString(__in UINT_PTR idCmd, __in UINT uType, __reserved UINT *pReserved, __out_awcount(!(uType & GCS_UNICODE) , cchMax) LPSTR pszName, __in UINT cchMax);

public:



};

OBJECT_ENTRY_AUTO(__uuidof(LdContextMenu), CLdContextMenu)

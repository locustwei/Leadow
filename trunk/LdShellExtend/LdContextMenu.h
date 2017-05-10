/*!
 * \file LdContextMenu.h
 * \date 2017/04/28 11:25
 *
 * \author asa
 * 
 *
 * WindowsÓÒ¼ü²Ëµ¥²å¼þ¡£
 *
 * 
 *
 * 
*/

#pragma once
#include "resource.h"       // Ö÷·ûºÅ
#include "LdShellExtend_i.h"
#include "LdStructs.h"

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
		m_ContextMenuType = SHELL_EXTEND_TYPE_NONE;
		m_SelectFiles = NULL;
		m_SelectCount = 0;
	}

	~CLdContextMenu()
	{
		for(int i=0; i<m_SelectCount; i++)
			delete [] m_SelectFiles[i];
		if(m_SelectFiles)
			delete [] m_SelectFiles;
		m_SelectFiles = NULL;
		m_SelectCount = 0;
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
private:
	SHELL_EXTEND_TYPE m_ContextMenuType;
	LPTSTR* m_SelectFiles;
	int m_SelectCount;

	HMENU CreateAllFileMenum(UINT& idCmdFirst);
	HMENU CreateFolderMenum(UINT& idCmdFirst);
	HMENU CreateDriveMenum(UINT& idCmdFirst);
	HMENU CreateDirbkgMenum(UINT& idCmdFirst);
};

OBJECT_ENTRY_AUTO(__uuidof(LdContextMenu), CLdContextMenu)

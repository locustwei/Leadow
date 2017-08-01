#include "stdafx.h"
#include "ShellTree.h"

namespace DuiLib {

	CShellTree::CShellTree()
	{
	}
	CShellTree::~CShellTree()
	{
	}

	void CShellTree::AttanchControl(CControlUI* pCtrl)
	{
		assert(pCtrl->GetInterface(DUI_CTR_TREEVIEW));
		__super::AttanchControl(pCtrl);
		GetRootItems();
	}

	void CShellTree::SetNodeSkin(TCHAR* pSkin)
	{
		m_NodeSkin = pSkin;
	}

	BOOL CShellTree::GetRootItems()
	{
		LPITEMIDLIST pidl;

		if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
		{
			return FALSE;
		}

		// Get the desktop's IShellFolder:
		LPSHELLFOLDER pDesktop;
		if (FAILED(SHGetDesktopFolder(&pDesktop)))
		{
			return FALSE;
		}

		CTreeNodeUI* pNode = BuildTreeNode();
		PSHELLITEMINFO pInfo = (PSHELLITEMINFO)GlobalAlloc(GPTR, sizeof(SHELLITEMINFO));
		pInfo->pidlRel = pidl;
		pNode->SetTag((UINT_PTR)pInfo);

		pNode->SetAttribute(_T("text"), GetShllItemText(pidl));

		pNode->Expand(true);

		pDesktop->Release();
		return TRUE;
	}

	CTreeNodeUI* CShellTree::BuildTreeNode()
	{
		CTreeNodeUI* result = nullptr;
		if(m_NodeSkin)
		{
			CDialogBuilder builder;
			result = (CTreeNodeUI*)builder.Create(m_NodeSkin, NULL, nullptr, GetTree()->GetManager(), NULL);
		}
		return result;
	}

	CDuiString CShellTree::GetShllItemText(LPITEMIDLIST pItem)
	{
		SHFILEINFO sfi;

		if (SHGetFileInfo((LPCTSTR)pItem, 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
		{
			return sfi.szDisplayName;
		}

		return _T("???");
	}

	CTreeViewUI* CShellTree::GetTree()
	{
		return (CTreeViewUI*)m_Ctrl;
	}


}

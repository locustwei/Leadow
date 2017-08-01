#pragma once
#include <Shlwapi.h>
#include <ShlObj.h>

namespace DuiLib {

	class DUILIB_API CShellTree :
		public CFramNotifyPump
	{
	public:
		CShellTree();
		~CShellTree();
		void AttanchControl(CControlUI* pCtrl) override;
		void SetNodeSkin(TCHAR* pSkin);
		
		DUI_DECLARE_MESSAGE_MAP()

	private:
		typedef struct SHELLITEMINFO
		{
			LPSHELLFOLDER  pParentFolder;
			LPITEMIDLIST   pidlFQ;
			LPITEMIDLIST   pidlRel;
		}*PSHELLITEMINFO;

		BOOL GetRootItems();
		CTreeNodeUI* BuildTreeNode();
		CDuiString GetShllItemText(LPITEMIDLIST pItem);
	protected:
		CTreeViewUI* GetTree();
		TCHAR* m_NodeSkin;
	};

}
#include "stdafx.h"
#include "ShFileView.h"

CShFileViewUI::CShFileViewUI():
	m_Columes()
{
	lstFile = nullptr;
	m_HeaderAdded = false;
	m_ItemSkin = nullptr;
}


CShFileViewUI::~CShFileViewUI()
{
	for(int i=0; i<m_Columes.GetCount();i++)
	{
		delete m_Columes[i]->szName;
		delete m_Columes[i];
	}
}

void CShFileViewUI::AddRecord(CLdArray<TCHAR*>* values)
{
	if (!m_HeaderAdded)
		AddLstViewHeader(values->GetCount());

	if (m_ItemSkin)
	{
		CDialogBuilder builder;
		CListContainerElementUI* pItem = (CListContainerElementUI*)builder.Create(m_ItemSkin, NULL, nullptr, m_Control->GetManager(), NULL);
		assert(pItem);
		lstFile->Add(pItem);
		for (int i = 1; i < values->GetCount(); i++)
		{
			CControlUI* ctrl = pItem->GetItemAt(i-1);
			if (ctrl)
			{
				CControlUI* cap = ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("caption"), 0);
				if (cap)
					cap->SetText(values->Get(i));
				else
					ctrl->SetText(values->Get(i));
			}
			else
			{
				CLabelUI* cap = new CLabelUI();
				cap->SetText(values->Get(i));
			}
		}
	}
	else
	{
		CListTextElementUI* pItem = new CListTextElementUI();
		lstFile->Add(pItem);
		for (int i = 0; i < values->GetCount(); i++)
		{
			pItem->SetText(0, values->Get(i));
		}
	}
}

DWORD CShFileViewUI::AddFile(TCHAR* lpFullName)
{
	if (m_Columes.GetCount() == 0)
	{
		CSHFolders::EnumFolderColumes(lpFullName, this, 0);
	}

	CLdArray<TCHAR*> values;
	CSHFolders::GetFileAttributeValue(lpFullName, &values);
	AddRecord(&values);
	for (int i = 0; i < values.GetCount(); i++)
		delete values[i];

	return 0;
}

DWORD CShFileViewUI::AddFolder(TCHAR* lpFullName)
{
	if (m_Columes.GetCount() == 0)
	{
		CSHFolders::EnumFolderColumes(lpFullName, this, 0);
	}
	CSHFolders::EnumFolderObjects(lpFullName, this, 0);
	return 0;
}

DWORD CShFileViewUI::AddFolder(int nFolderCSIDL)
{
	if (m_Columes.GetCount() == 0)
	{
		CSHFolders::EnumFolderColumes(nFolderCSIDL, this, 0);
	}
	CSHFolders::EnumFolderObjects(nFolderCSIDL, this, 0);
	return 0;
}

void CShFileViewUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CShFileViewUI::OnItemClick(TNotifyUI & msg)
{
	
}

DUI_BEGIN_MESSAGE_MAP(CShFileViewUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//lstFile添加列头
void CShFileViewUI::AddLstViewHeader(int ncount)
{
	if (m_HeaderAdded)
		return;

	CListHeaderUI * pHeader = lstFile->GetHeader();
	//lstFile->Add(pHeader);
	pHeader->SetAttribute(_T("style"), _T("lstHeader_file"));

	for(int i=0; i<ncount; i++)
	{
		if (i >= m_Columes.GetCount())
			break;
		CListHeaderItemUI* pItem = new CListHeaderItemUI();
		pHeader->Add(pItem);
		pItem->SetAttribute(_T("style"), _T("lstHeaderitem_file"));

		pItem->SetText(m_Columes[i]->szName);
		pItem->SetFixedWidth(m_Columes[i]->cxChar * 8);

	}
	m_HeaderAdded = true;
}
//找到虚拟文件并与实际文件对应，
BOOL CShFileViewUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	AddRecord(pData);

	return true;
}
//获取列信息暂存
BOOL CShFileViewUI::GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param)
{
	PSH_HEAD_INFO p = new SH_HEAD_INFO;
	p->cxChar = pData->cxChar;
	p->fmt = pData->fmt;
	p->szName = new TCHAR[_tcslen(pData->szName)+1];
	_tcscpy(p->szName, pData->szName);
	m_Columes.Add(p);
	return true;
};

void CShFileViewUI::OnInit()
{
	lstFile = static_cast<CListUI*>(GetUI()->FindControl(CDuiUtils::FindControlByNameProc, _T("listview"), 0));
}


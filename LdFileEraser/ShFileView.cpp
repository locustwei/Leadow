#include "stdafx.h"
#include "ShFileView.h"

CShFileViewUI::CShFileViewUI():
	m_Columes(),
	m_ErasureFiles(500),
	m_EreaserThreads()
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

	for(int i=0; i<m_ErasureFiles.GetCount(); i++)
	{
		PERASURE_FILE_DATA p;
		if(m_ErasureFiles.GetValueAt(i, p))
			delete p;
	}

	m_EreaserThreads.StopThreads();
	while (m_EreaserThreads.GetCount() > 0)
		Sleep(10); //等待所有线程结束

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
		for (int i = 0; i < values->GetCount(); i++)
		{
			CControlUI* ctrl = pItem->GetItemAt(i);
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
		lstFile->Add(pItem);
	}
	else
	{
		CListTextElementUI* pItem = new CListTextElementUI();
		for (int i = 0; i < values->GetCount(); i++)
		{
			pItem->SetText(0, values->Get(i));
		}
		lstFile->Add(pItem);
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
	return 0;
}

DWORD CShFileViewUI::AddFolder(TCHAR* lpFullName)
{
	CSHFolders::EnumFolderObjects(lpFullName, this, 0);
}

void CShFileViewUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CShFileViewUI::OnItemClick(TNotifyUI & msg)
{
	
}

DUI_BEGIN_MESSAGE_MAP(CShFileViewUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//lstFile添加列头
void CShFileViewUI::AddLstViewHeader(int ncount)
{
	CListHeaderUI* pHeader = lstFile->GetHeader();
	if(pHeader == nullptr)
	{
		pHeader = new CListHeaderUI();
		pHeader->SetAttribute(_T("style"), _T("lstHeader_file"));
	}

	for(int i=0; i<ncount; i++)
	{
		if (i >= m_Columes.GetCount())
			break;
		CListHeaderItemUI* pItem = new CListHeaderItemUI();
		pItem->SetAttribute(_T("style"), _T("lstHeaderitem_file"));

		pItem->SetText(m_Columes[i]->szName);
		pItem->SetFixedWidth(m_Columes[i]->cxChar * 10);
		pHeader->Add(pItem);

	}
}
//找到虚拟文件并与实际文件对应，
BOOL CShFileViewUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
	if (pData->GetCount() > lstFile->GetHeader()->GetCount() + 1)
		AddLstViewHeader(pData->GetCount() + 1);

	PERASURE_FILE_DATA pinfo = nullptr;
	if(m_ErasureFiles.Find(fi.szDisplayName, pinfo))
	{
		//添加到lstFile中
		CListContainerElementUI* item = static_cast<CListContainerElementUI*>(lstFile->GetItemAt(0));
		if (item->GetTag() != 0)
		{
			item = static_cast<CListContainerElementUI*>(item->CloneNew());
			lstFile->Add(item);
		}
		item->SetVisible(true);
		item->SetTag((UINT_PTR)pinfo);
		if(pinfo)
			pinfo->ListRow = item;
		for (int i = 1; i<pData->GetCount(); i++)
		{
			CControlUI* ui = item->GetItemAt(i-1);
			if (ui)
			{
				CControlUI* cap = ui->FindSubControl(_T("caption"));
				if (cap)
					cap->SetText(pData->Get(i));
				else
					ui->SetText(pData->Get(i));

			}
		}
	}
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

void CShFileViewUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag() && m_EreaserThreads.GetCount() == 0)
			m_EreaserThreads.AddThread(this, 0);
		else
		{
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

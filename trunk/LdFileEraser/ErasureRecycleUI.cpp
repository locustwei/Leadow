#include "stdafx.h"
#include "ErasureRecycleUI.h"
#include "ErasureProcess.h"
#include <thread>


CErasureRecycleUI::CErasureRecycleUI():
	m_Columes(),
	m_RecycledFiles(200)
{
	btnOk = nullptr;
	lstFile = nullptr;
	BuildXml(_T("erasure\\erasurerecycle.xml"), nullptr);
}


CErasureRecycleUI::~CErasureRecycleUI()
{
	for(int i=0; i<m_Columes.GetCount();i++)
	{
		delete m_Columes[i]->szName;
		delete m_Columes[i];
	}
	m_Columes.Clear();
	for(int i=0; i<m_RecycledFiles.GetSize(); i++)
	{
		LPWIN32_FIND_DATA p;
		if(m_RecycledFiles.GetValueAt(i, p))
			delete p;
	}
}

VOID CErasureRecycleUI::ThreadRun(CThread* Sender, WPARAM Param)
{
	CListContainerElementUI* ui = reinterpret_cast<CListContainerElementUI*>(Param);
	LPWIN32_FIND_DATA pinfo = reinterpret_cast<LPWIN32_FIND_DATA>(ui->GetTag());
	CErasure erasure;
	CErasureProcess* pProcess = new CErasureProcess();
	pProcess->ui = (CProgressUI*)ui->FindSubControl(_T("progress"));
	if (pProcess->ui)
		pProcess->ui->SetVisible(true);
	erasure.FileErasure(pinfo->cFileName, CErasureMethod::Pseudorandom(), pProcess);
	delete pProcess;
}

VOID CErasureRecycleUI::OnThreadInit(CThread* Sender, WPARAM Param)
{
	;
}

VOID CErasureRecycleUI::OnThreadTerminated(CThread* Sender, WPARAM Param)
{
	CListContainerElementUI* ui = reinterpret_cast<CListContainerElementUI*>(Param);
	lstFile->Remove(ui);
}

void CErasureRecycleUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureRecycleUI::OnItemClick(TNotifyUI & msg)
{
	
}


DUI_BEGIN_MESSAGE_MAP(CErasureRecycleUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	CLdString s = (LPTSTR)Param;
	LPWIN32_FIND_DATA p = new WIN32_FIND_DATA;
	MoveMemory(p, pData, sizeof(WIN32_FIND_DATA));
	s += p->cFileName;
	m_RecycledFiles.Set(p->cFileName, p);
	s.CopyTo(p->cFileName);
	return true;
}

void CErasureRecycleUI::AddLstViewHeader(int ncount)
{
	CControlUI* col = lstFile->GetHeader()->GetItemAt(0);
	col->SetVisible(true);
	while(lstFile->GetHeader()->GetCount()<ncount)
	{
		CControlUI* col1 = col->CloneNew();
		lstFile->GetHeader()->Add(col1);
	}
	for(int i=0; i<ncount; i++)
	{
		if (i >= m_Columes.GetCount())
			break;
		col = lstFile->GetHeader()->GetItemAt(i);
		col->SetText(m_Columes[i]->szName);
		col->SetFixedWidth(m_Columes[i]->cxChar * 10);
	}
}

BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
	if (pData->GetCount() > lstFile->GetHeader()->GetCount() + 1)
		AddLstViewHeader(pData->GetCount() + 1);

	LPWIN32_FIND_DATA pinfo;

	if(m_RecycledFiles.Find(fi.szDisplayName, pinfo))
	{
		CListContainerElementUI* item = static_cast<CListContainerElementUI*>(lstFile->GetItemAt(0));
		if (item->GetTag() != 0)
		{
			item = static_cast<CListContainerElementUI*>(item->CloneNew());
			lstFile->Add(item);
		}
		item->SetVisible(true);
		item->SetTag((UINT_PTR)pinfo);

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

BOOL CErasureRecycleUI::GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param)
{
	PSH_HEAD_INFO p = new SH_HEAD_INFO;
	p->cxChar = pData->cxChar;
	p->fmt = pData->fmt;
	p->szName = new TCHAR[_tcslen(pData->szName)+1];
	_tcscpy(p->szName, pData->szName);
	m_Columes.Add(p);
	return true;
};

BOOL CErasureRecycleUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CLdArray<CLdString*>* pVolumes = (CLdArray<CLdString*>*)Param;
	pVolumes->Add(new CLdString(pData));
	return TRUE;
};

void CErasureRecycleUI::EnumRecyleFiels()
{
	DWORD oldMode;
	CLdArray<CLdString*> Volumes;
	CLdString sid;
	CLdString recyclePath;

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);
	WIN_OS_TYPE os = GetOsType();
	if (GetCurrentUserSID(sid) != 0)
		return;
	CVolumeUtils::MountedVolumes(this, (UINT_PTR)&Volumes);

	for(int i=0; i<Volumes.GetCount(); i++)
	{
		recyclePath = Volumes[i]->GetData();
		if (os >= Windows_Vista)
			recyclePath += _T("$RECYCLE.BIN");
		else
			recyclePath += _T("RECYCLE");

		switch (CVolumeUtils::GetVolumeFileSystem(Volumes[i]->GetData()))
		{
		case FS_NTFS:
			if (os < Windows_Vista)
				recyclePath += _T("R");
			recyclePath += '\\';
			recyclePath += sid;
			break;
		default:
			if (os < Windows_Vista)
				recyclePath += _T("D");
		}

		recyclePath += _T("\\");

		CFileUtils::FindFile(recyclePath, L"*", true, this, (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}

	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::OnInit()
{
	btnOk = static_cast<CButtonUI*>(GetUI()->FindSubControl(_T("btnOk")));
	lstFile = static_cast<CListUI*>(GetUI()->FindSubControl(_T("listview")));

	CSHFolders::EnumFolderColumes(CSIDL_BITBUCKET, nullptr, this, 0);

	EnumRecyleFiels();

	CSHFolders::EnumFolderObjects(CSIDL_BITBUCKET, nullptr, this, 0);
}

void CErasureRecycleUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		CLdString s;
		for (int i = 0; i < lstFile->GetCount(); i++)
		{
			CControlUI* control = lstFile->GetItemAt(i);
			COptionUI* sel = (COptionUI*)control->FindSubControl(_T("select"));
			if(!sel || (sel && sel->IsSelected()))
			{
				CThread* thread = new CThread(this);
				thread->Start(WPARAM(control));
			}
		}
	}
}

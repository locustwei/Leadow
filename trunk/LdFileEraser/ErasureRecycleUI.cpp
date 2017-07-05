#include "stdafx.h"
#include "ErasureRecycleUI.h"
#include "ErasureProcess.h"
#include <thread>


CThread* CEreaserThrads::AddThread(IThreadRunable* run, UINT_PTR Param)
{
	EnterCriticalSection(&cs);
	CErasureThread* result = new CErasureThread(run);
	Add(result);
	result->Start(Param);
	LeaveCriticalSection(&cs);
	return result;
}

CErasureRecycleUI::CErasureRecycleUI():
	m_Columes(),
	m_RecycledFiles(500),
	m_EreaserThreads()
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
	for(int i=0; i<m_RecycledFiles.GetCount(); i++)
	{
		PRECYCLE_FILE_DATA p;
		if(m_RecycledFiles.GetValueAt(i, p))
			delete p;
	}
	m_EreaserThreads.StopThreads();
	while (m_EreaserThreads.GetCount() > 0)
		Sleep(10);
}

void CErasureRecycleUI::ErasureAllFiles(CThread* Sender)
{
	m_ThreadCount = 0;
	for (int i = 0; i < m_RecycledFiles.GetCount(); i++)
	{//先擦除文件

		while (m_EreaserThreads.GetCount() >= MAX_THREAD_COUNT)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}

		if (Sender->GetTerminated())
			break;

		PRECYCLE_FILE_DATA pinfo;
		CLdString s = m_RecycledFiles.GetAt(i);

		m_RecycledFiles.GetValueAt(i, pinfo);
		if (!pinfo->IsDirectory)
		{
			m_EreaserThreads.AddThread(this, UINT_PTR(m_RecycledFiles.GetAt(i)));
		}
	}
	if (Sender->GetTerminated())
		return;

	for (int i = 0; i < m_RecycledFiles.GetCount(); i++)
	{//目录
		while (m_EreaserThreads.GetCount() >= MAX_THREAD_COUNT)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}

		if (Sender->GetTerminated())
			break;

		PRECYCLE_FILE_DATA pinfo;
		m_RecycledFiles.GetValueAt(i, pinfo);
		if (pinfo->IsDirectory)
		{
			m_EreaserThreads.AddThread(this, UINT_PTR(m_RecycledFiles.GetAt(i)));
		}
	}
}

void CErasureRecycleUI::ErasureSingleFile(CThread* Sender, TCHAR* Key)
{
	if (Sender->GetTerminated())
		return;
	DebugOutput(L"++%s\n", Key);
	PRECYCLE_FILE_DATA pinfo;
	m_RecycledFiles.Find(Key, pinfo);
	CErasureThread* pProcess = (CErasureThread*)Sender;

	CListContainerElementUI* ui = (CListContainerElementUI*)pinfo->ListRow;
	if (ui)
	{
		pProcess->ui = (CProgressUI*)ui->FindSubControl(_T("progress"));
		if (pProcess->ui)
			pProcess->ui->SetVisible(true);
	}
	CErasure erasure;
	DWORD r = erasure.FileErasure(pinfo->cFileName, CErasureMethod::Pseudorandom(), pProcess);
	
	if(r == 0)
	{
		m_RecycledFiles.Remove(Key);
		if (ui)
			CLdApp::Send2MainThread(this, (UINT_PTR)ui);
	}
	DebugOutput(L"--%s\n", Key);

}

VOID CErasureRecycleUI::ThreadRun(CThread* Sender, UINT_PTR Param)
{
	if (Param == 0)
		ErasureAllFiles(Sender);
	else
		ErasureSingleFile(Sender, (TCHAR*)Param);
}

VOID CErasureRecycleUI::OnThreadInit(CThread* Sender, UINT_PTR Param)
{
	if (Param == 0)
	{
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
	}
}

VOID CErasureRecycleUI::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
{
	m_EreaserThreads.RemoveThread(Sender);
	if (m_EreaserThreads.GetCount() == 0)
	{
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"OK");
	}
	DebugOutput(L"--%s\n", (TCHAR*)Param);
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

BOOL CErasureRecycleUI::GernalCallback_Callback(LPVOID pData, UINT_PTR Param)
{
	CListContainerElementUI* ui = reinterpret_cast<CListContainerElementUI*>(Param);
	lstFile->Remove(ui);
	return true;
}

BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	//if (_tcsicmp(pData->cFileName, _T("desktop.ini")) == 0)
		//return true;
	CLdString s = (LPTSTR)Param;
	PRECYCLE_FILE_DATA p = new RECYCLE_FILE_DATA;
	s += pData->cFileName;
	s.CopyTo(p->cFileName);
	p->ListRow = nullptr;
	p->IsDirectory = pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY == FILE_ATTRIBUTE_DIRECTORY;
	m_RecycledFiles.Set(pData->cFileName, p);
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
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
	if (pData->GetCount() > lstFile->GetHeader()->GetCount() + 1)
		AddLstViewHeader(pData->GetCount() + 1);

	PRECYCLE_FILE_DATA pinfo = nullptr;
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
		if(pinfo)
			pinfo->ListRow = item;
		for (int i = 1; i<pData->GetCount(); i++)
		{
			CControlUI* ui = item->GetItemAt(i-1);
			if (ui)
			{
				CControlUI* cap = ui->FindSubControl(_T("caption"));
				CLdString s;
				s.Format(L"%d %s", lstFile->GetCount(), pData->Get(i));
				if (cap)
					cap->SetText(s);
				else
					ui->SetText(s);

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
		if (!btnOk->GetTag() && m_EreaserThreads.GetCount() == 0)
			m_EreaserThreads.AddThread(this, 0);
		else
		{
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

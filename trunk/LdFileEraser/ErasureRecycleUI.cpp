#include "stdafx.h"
#include "ErasureRecycleUI.h"
#include "ErasureProcess.h"
#include <thread>


CErasureRecycleUI::CErasureRecycleUI()
{
	btnOk = nullptr;
	lstFile = nullptr;
	BuildXml(_T("erasure\\erasurerecycle.xml"), nullptr);
}


CErasureRecycleUI::~CErasureRecycleUI()
{
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
	LPWIN32_FIND_DATA pinfo = reinterpret_cast<LPWIN32_FIND_DATA>(ui->GetTag());
	delete pinfo;
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

void CErasureRecycleUI::AddErasureFile(TCHAR* lpOrgFileName, LPWIN32_FIND_DATA pData)
{
	CListContainerElementUI* item = static_cast<CListContainerElementUI*>(lstFile->GetItemAt(0));
	if (item->GetTag() != 0)
	{
		item = static_cast<CListContainerElementUI*>(item->CloneNew());
		lstFile->Add(item);
	}
	item->SetVisible(true);
	LPWIN32_FIND_DATA pinfo = new WIN32_FIND_DATA();
	CopyMemory(pinfo, pData, sizeof(WIN32_FIND_DATA));
	item->SetTag((UINT_PTR)pinfo);
	item->SetSubControlText(_T("filename"), lpOrgFileName);

	item->SetSubControlText(_T("filesize"), CFileInfo::FormatFileSize(pinfo->nFileSizeLow));
	item->SetSubControlText(_T("createtime"), CDateTimeUtils::DateTimeToString(pinfo->ftCreationTime));
}

BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	SHFILEINFO		fi = { 0 };
	CLdString s = (LPTSTR)Param;
	s += pData->cFileName;
	HRESULT hr = SHGetFileInfo(s, 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME);
	s.CopyTo(pData->cFileName);
	AddErasureFile(fi.szDisplayName, pData);
	return true;
};

class CVolumeCallbackImp : public IGernalCallback<TCHAR*>
{
public:
	TCHAR* szRecycle;
	WIN_OS_TYPE os;
	CLdString sid;
	IGernalCallback<LPWIN32_FIND_DATA>* callback;

	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override
	{
		CLdString recyclePath = pData;
		recyclePath += szRecycle;

		switch (CVolumeUtils::GetVolumeFileSystem(pData))
		{
		case FS_NTFS:
			if (os < Windows_Vista)
				recyclePath += _T("R");
			recyclePath += "\\";
			recyclePath += sid;
			break;
		default:
			if (os < Windows_Vista)
				recyclePath += _T("D");
		}

		recyclePath += _T("\\");

		CFileUtils::FindFile(recyclePath, L"*", true, callback, (UINT_PTR)recyclePath.GetData());

		return TRUE;
	};
};

void CErasureRecycleUI::EnumRecyleFiels()
{
	DWORD oldMode;
	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);

	CVolumeCallbackImp VolumeCallback;
	VolumeCallback.os = GetOsType();
	if(GetCurrentUserSID(VolumeCallback.sid) != 0)
		return ;

	if (VolumeCallback.os >= Windows_Vista)
		VolumeCallback.szRecycle = _T("$RECYCLE.BIN");
	else
	{
		VolumeCallback.szRecycle = _T("RECYCLE");
	}
	VolumeCallback.callback = this;
	CVolumeUtils::MountedVolumes(&VolumeCallback, 0);
	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::OnInit()
{
	btnOk = static_cast<CButtonUI*>(GetUI()->FindSubControl(_T("btnOk")));
	lstFile = static_cast<CListUI*>(GetUI()->FindSubControl(_T("listview")));

	EnumRecyleFiels();
}

void CErasureRecycleUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		for (int i = 0; i < lstFile->GetCount(); i++)
		{
			CControlUI* control = lstFile->GetItemAt(i);
			COptionUI* selectui = (COptionUI*)control->FindSubControl(_T("select"));
			if(selectui->IsSelected())
			{
				CThread* thread = new CThread(this);
				thread->Start(WPARAM(control));
			}
			//CErasure erasure;
			//erasure.FileErasure(pinfo->GetFileName(), CErasureMethod::Pseudorandom(), this);
		}
	}
}

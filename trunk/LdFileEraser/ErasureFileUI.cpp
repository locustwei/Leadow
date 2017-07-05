#include "stdafx.h"
#include "ErasureFileUI.h"
#include "ErasureProcess.h"


CErasureFileUI::CErasureFileUI()
{
	btnOpenFile = nullptr;
	btnOk = nullptr;
	lstFile = nullptr;
	BuildXml(_T("erasure\\erasurefile.xml"), nullptr);
}


CErasureFileUI::~CErasureFileUI()
{

}

VOID CErasureFileUI::ThreadRun(CThread* Sender, WPARAM Param)
{
	CListContainerElementUI* ui = (CListContainerElementUI*)Param;
	CFileInfo* pinfo = (CFileInfo*)ui->GetTag();
	CErasure erasure;
	CErasureThread* pProcess = (CErasureThread*)Sender;
	pProcess->ui = (CProgressUI*)ui->FindSubControl(_T("progress"));
	if (pProcess->ui)
		pProcess->ui->SetVisible(true);
	erasure.FileErasure(pinfo->GetFileName().GetData(), CErasureMethod::Pseudorandom(), pProcess);
	delete pProcess;
}

VOID CErasureFileUI::OnThreadInit(CThread* Sender, WPARAM Param)
{
	;
}

VOID CErasureFileUI::OnThreadTerminated(CThread* Sender, WPARAM Param)
{
	;
}

void CErasureFileUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureFileUI::OnItemClick(TNotifyUI & msg)
{
	
}


DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::AddErasureFile(CLdString& filename)
{
	CListContainerElementUI* item = static_cast<CListContainerElementUI*>(lstFile->GetItemAt(0));
	if (item->GetTag() != 0)
	{
		item = static_cast<CListContainerElementUI*>(item->CloneNew());
		lstFile->Add(item);
	}
	item->SetVisible(true);
	CFileInfo* pinfo = new CFileInfo(filename.GetData());
	item->SetTag((UINT_PTR)pinfo);
	item->SetSubControlText(_T("filename"), filename);

	item->SetSubControlText(_T("filesize"), CFileInfo::FormatFileSize(pinfo->GetDataSize()));
	item->SetSubControlText(_T("createtime"), CDateTimeUtils::DateTimeToString(pinfo->GetCreateTime()));
}

void CErasureFileUI::OnInit()
{
	btnOpenFile = static_cast<CButtonUI*>(GetUI()->FindSubControl(_T("openfile")));
	btnOk = static_cast<CButtonUI*>(GetUI()->FindSubControl(_T("btnOk")));
	lstFile = static_cast<CListUI*>(GetUI()->FindSubControl(_T("listview")));
	lstFile->GetItemAt(0);
}

void CErasureFileUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOpenFile)
	{
		CDlgGetFileName dlg;
		dlg.SetOption(CDlgGetFileName::OPEN_FILE_OPTION | OFN_ALLOWMULTISELECT);
		if (dlg.OpenFile(GetUI()->GetManager()->GetPaintWindow()))
		{
			for (int i = 0; i < dlg.GetFileCount(); i++)
			{
				CLdString filename = dlg.GetFileName(i);
				AddErasureFile(filename);
			}
		}
	}
	else if (msg.pSender == btnOk)
	{
		for (int i = 0; i < lstFile->GetCount(); i++)
		{
			CThread* thread = new CThread(this);
			thread->Start(WPARAM(lstFile->GetItemAt(i)));
			//CErasure erasure;
			//erasure.FileErasure(pinfo->GetFileName(), CErasureMethod::Pseudorandom(), this);
		}
	}
}

#include "stdafx.h"
#include "ErasureFileUI.h"


CErasureFileUI::CErasureFileUI():
	m_Files()
{
	btnOpenFile = NULL;
	btnOk = NULL;
	lstFile = NULL;
	BuildXml(_T("erasure\\filelistview.xml"), NULL);
}


CErasureFileUI::~CErasureFileUI()
{
	for (int it = 0; it < m_Files.GetCount(); it++)
	{
		delete m_Files[it];
	}
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
	CListTextElementUI* element = new CListTextElementUI();
	
	lstFile->Add(element);
	m_Files.Add(new CFileInfo(filename.GetData()));
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
	}
}

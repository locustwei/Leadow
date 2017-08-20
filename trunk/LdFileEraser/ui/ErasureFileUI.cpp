#include "stdafx.h"
#include "ErasureFileUI.h"

CErasureFileUI::CErasureFileUI() :
	m_ErasureFiles(),
	m_EreaserThreads(this)
{
	btnOpenFile = nullptr;
	btnOk = nullptr;
	lstFile = nullptr;
	m_Name = _T("ErasureFileUI");
	m_ItemSkin = _T("erasure\\listitem_file.xml");
}

CErasureFileUI::~CErasureFileUI()
{
	
}

void CErasureFileUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureFileUI::OnItemClick(TNotifyUI & msg)
{
	
}

CVirtualFile* CErasureFileUI::AddEraseFile(TCHAR* file_name)
{
	CFileInfo* info;
	if (CFileUtils::IsDirectoryExists(file_name))
		info = new CFolderInfo();
	else
		info = new CFileInfo();
	info->SetFileName(file_name);

	PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
	ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
	info->SetTag((UINT_PTR)p);

	m_ErasureFiles.Add(info);
}

void CErasureFileUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOpenFile = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("openfile"), 0));
	btnOk = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));
	CSHFolders::EnumFolderColumes(L"C:\\", this, 0);
}

bool CErasureFileUI::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	return true;
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::OnClick(TNotifyUI& msg)
{
	if(msg.pSender == btnOpenFile)
	{
		CDlgGetFileName dlg;
		dlg.SetOption(CDlgGetFileName::OPEN_FILE_OPTION | OFN_ALLOWMULTISELECT);

		if(dlg.OpenFile(m_Ctrl->GetManager()->GetPaintWindow(), dft_file_folder))
		{
			for(int i=0; i<dlg.GetFileCount();i++)
			{
				

				CVirtualFile* pFile = AddEraseFile(fileName);

				AddFile(dlg.GetFileName(i));
			}
		};
	}
}

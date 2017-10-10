#include "stdafx.h"
#include "ErasureFileUI.h"
#include "EraserUI.h"

CErasureFileUI::CErasureFileUI() :
	//m_file_map()
	m_ErasureFile()
{
	btnOpenFile = nullptr;
	btnOk = nullptr;
	lstFile = nullptr;
	m_Name = _T("ErasureFileUI");
	m_ItemSkin = _T("erasure/listitem_file.xml");
}

CErasureFileUI::~CErasureFileUI()
{
	//m_EreaserThreads.StopThreads();
	FreeEraseFiles(m_ErasureFile.GetFiles());
}

void CErasureFileUI::FreeEraseFiles(CLdArray<CVirtualFile*>* files)
{
//	for (int i = 0; i<files->GetCount(); i++)
//	{
//		CVirtualFile * file = files->Get(i);
//		if (file)
//		{
//			file->SetTag(0);
//			if (file->GetFileType() == vft_folder)
//				FreeEraseFiles(file->GetFiles());
//		}
//
//	}
}
//设置文件的目录指向，擦除时更新隶属文件夹的进度
DWORD CErasureFileUI::SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui)
{
	pFile->SetTag((UINT_PTR)ui);
	return 0;
}

bool CErasureFileUI::OnAfterColumePaint(PVOID Param)
{
	PUI_PAINT_PARAM pPaint = (PUI_PAINT_PARAM)Param;
	UINT percent = (UINT)pPaint->sender->GetTag();
	if (percent == 0)
		return true;
	RECT rect = pPaint->sender->GetPos();
	rect.bottom = rect.top + 20;
	rect.right = rect.left + ((rect.right - rect.left) * percent) / 100;
	CRenderEngine::DrawColor(pPaint->hDc, rect, 0x8008E0E0);
	return true;
}

CVirtualFile* CErasureFileUI::AddEraseFile(TCHAR* file_name)
{
	CFileInfo* info;
	if (CFileUtils::IsDirectoryExists(file_name))
	{
		info = new CFolderInfo();
		info->SetFileName(file_name);
		((CFolderInfo*)info)->FindFiles(true);
	}
	else
	{
		info = new CFileInfo();
		info->SetFileName(file_name);		
	}
	SetFolderFilesData(info, nullptr);

	m_ErasureFile.AddFile(info);
	return info;
}

void CErasureFileUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOpenFile = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("openfile"), 0));
	btnOk = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));

	//用系统目录为模板,添加默认列.
	TCHAR Path[MAX_PATH] = { 0 };
	GetSystemDirectory(Path, MAX_PATH);
	CSHFolders::EnumFolderColumes(Path, this, 0);

	AddLstViewHeader(8);
}

void CErasureFileUI::DeleteErasuredFile(CLdArray<CVirtualFile*>* files)
{
	
}

void CErasureFileUI::UpdateEraseProgressMsg(CControlUI* ui, int Percent)
{
	CDuiString str;

	CControlUI* col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
	if (col)
	{				
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			/*if (pData->nStatus == efs_error)
			{
				str.Format(_T("发生错误， 错误代码%x"), pData->nErrorCode);
				desc->SetBkColor(0xFFFF0000);
			}
			else
			{
				if (Percent < 100)
					str.Format(_T("已擦除%d个文件"), pData->nErasued);
				else
				{
					str = _T("已完成");
					desc->SetBkColor(0xFF00FFFF);
				}
			}*/
			desc->SetText(str);
		}
		col->SetTag(Percent);
		col->NeedUpdate();
	}
}

bool CErasureFileUI::EraserThreadCallback(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue)
{
	CVirtualFile* p = m_ErasureFile.Find(FileName);
	if (!p)
		return false;
	CControlUI* ui = (CControlUI*)p->GetTag();
	switch (op)
	{
	case eto_start:  //总进度开始
		btnOk->SetTag(1);
		btnOk->SetText(L"取消");
		break;
	case eto_begin:  
		break;
	case eto_completed: //单个文件擦除完成 //设置擦除状态
		UpdateEraseProgressMsg(ui, 0);
		break;
	case eto_progress: //单个文件进度
		CControlUI* col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
		if (col)
		{
			if(dwValue > col->GetTag()) //多个线程更新当前进度，保留进度最大的那个。
				col->SetTag(dwValue);
			if (col->GetTag() >= 100)
				col->SetTag(0);
			col->NeedUpdate();
		}
		break;
	case eto_error:
		break;
	case eto_finished:
		DeleteErasuredFile(nullptr);
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"执行");
		break;
	default:
		break;
	}
	return true;
}

void CErasureFileUI::StatErase()
{
	//ExecuteFileErase(this, m_file_map.GetKeyArrary());
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::AddFileUI(CVirtualFile* pFile)
{
	//PFILE_ERASURE_DATA p = (PFILE_ERASURE_DATA)pFile->GetTag();
	CControlUI* ui = AddFile(pFile->GetFullName());

	CControlUI* col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
	if (col)
	{
		col->SetTag(0);
		col->OnAfterPaint += MakeDelegate(this, &CErasureFileUI::OnAfterColumePaint);
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			TCHAR s[MAX_PATH] = {0};
			CFileUtils::ExtractFilePath(pFile->GetFullName(), s);
			desc->SetText(s);
		}
	}
	col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
	if (col)
	{
		col->SetTag(0);
		col->OnAfterPaint += MakeDelegate(this, &CErasureFileUI::OnAfterColumePaint);
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			CLdString s, strSize;
			if (pFile->GetFileType() == vft_folder)
			{
				CFileUtils::FormatFileSize(pFile->GetDataSize(), strSize);
				s.Format(_T("包含数%d个文件（文件夹），合计%s"), ((CFolderInfo*)pFile)->GetFilesCount(true), strSize);
				desc->SetText(s);
			}
			else
			{
				CLdArray<CVirtualFile*>* streams = ((CFileInfo*)pFile)->GetADStreams();
				if (streams && streams->GetCount()>0)
				{
					INT64 nSize = 0;
					for(int i=0; i<streams->GetCount();i++)
					{
						nSize += streams->Get(i)->GetDataSize();
					}
					CFileUtils::FormatFileSize(nSize, strSize);
					s.Format(_T("文件有%d个交换数据流，合计%s"), streams->GetCount(), strSize);
					desc->SetText(s);
				};
			}
		}
	}
}

bool CErasureFileUI::GetViewHeader()
{//随便找个路径为模板，取得资源管理器的列信息
	TCHAR Path[MAX_PATH] = { 0 };
	GetSystemDirectory(Path, MAX_PATH);
	return CSHFolders::EnumFolderColumes(Path, this, 0) == 0;
}

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
				if (m_ErasureFile.Find(dlg.GetFileName(i), true, true) != nullptr)
					continue;
				CVirtualFile* pFile = AddEraseFile(dlg.GetFileName(i));
				AddFileUI(pFile);
			}
			m_ErasureFile.Sort();
		};
	}
	else if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag())
			StatErase();
		else
		{//擦除没有结束，取消当前任务
			//m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

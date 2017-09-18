#include "stdafx.h"
#include "ErasureFileUI.h"

CErasureFileUI::CErasureFileUI() :
	m_ErasureFile(),
	m_EreaserThreads(this)
{
	btnOpenFile = nullptr;
	btnOk = nullptr;
	lstFile = nullptr;
	m_Name = _T("ErasureFileUI");
	m_ItemSkin = _T("erasure/listitem_file.xml");
}

CErasureFileUI::~CErasureFileUI()
{
	m_EreaserThreads.StopThreads();
	FreeEraseFiles(m_ErasureFile.GetFiles());
}

void CErasureFileUI::FreeEraseFiles(CLdArray<CVirtualFile*>* files)
{
	for (int i = 0; i<files->GetCount(); i++)
	{
		CVirtualFile * file = files->Get(i);
		if (file)
		{
			PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
			if (pData)
			{
				delete pData;
			}
			file->SetTag(0);
			if (file->GetFileType() == vft_folder)
				FreeEraseFiles(file->GetFiles());
		}
	}
}
//设置文件的目录指向，擦除时更新隶属文件夹的进度
DWORD CErasureFileUI::SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui)
{
	DWORD nCount = 1;

	PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
	ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
	pFile->SetTag((UINT_PTR)p);
	p->ui = ui;
	if (pFile->GetFileType() == vft_folder)
	{
		//nCount = pFile->GetFiles()->GetCount();
		for (int i = 0; i < pFile->GetFiles()->GetCount(); i++)
		{
			CVirtualFile* file = pFile->GetFiles()->Get(i);
			nCount += SetFolderFilesData(file, ui);
		}
	}

	p->nCount = nCount ;

	return nCount;
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
	CRenderEngine::DrawColor(pPaint->hDc, rect, 0x80FFFF00);
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
	for (int i = files->GetCount() - 1; i >= 0; i--)
	{
		CVirtualFile* file = files->Get(i);
		PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
		if (pData && pData->nStatus == efs_erasured)
		{
			if (file->GetFileType() == vft_folder)
				DeleteErasuredFile(file->GetFiles());
			if (file->GetTag())
			{
				delete (PFILE_ERASURE_DATA)file->GetTag();
				file->SetTag(0);
			}
			files->Delete(i);
			delete file;
		}
	}
}

void CErasureFileUI::UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent)
{
	CDuiString str;

	CControlUI* col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
	if (col)
	{				
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			if (pData->nStatus == efs_error)
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
			}
			desc->SetText(str);
		}
		col->SetTag(Percent);
		col->NeedUpdate();
	}
}

bool CErasureFileUI::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	PFILE_ERASURE_DATA pEraserData;
	CVirtualFile* p;

	switch (op)
	{
	case eto_start:  //总进度开始
		btnOk->SetTag(1);
		btnOk->SetText(L"取消");
		break;
	case eto_begin:  
		break;
	case eto_completed: //单个文件擦除完成
						//设置擦除状态

		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
		{
			pEraserData->nStatus = efs_erasured;
		}
		else
		{
			pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
			//DebugOutput(L"finished error = %d %s\n", dwValue, pFile->GetFullName());
		}
		for (p = pFile; p != nullptr; p = p->GetFolder())
		{
			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());
			if (pEraserData)
				InterlockedIncrement(&pEraserData->nErasued);
			//更新所属文件夹进度
			if (pEraserData && pEraserData->ui)
			{
				int percent;

				if (pEraserData->nCount == 0 || pEraserData->nErasued > pEraserData->nCount)
					percent = 100;
				else
					percent = pEraserData->nErasued * 100 / pEraserData->nCount;
			
				UpdateEraseProgressMsg(pEraserData, pEraserData->ui, percent);
				break;
			}
		}
		break;
	case eto_progress: //单个文件进度
		for (p = pFile; p != nullptr; p = p->GetFolder())
		{
			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());
			if (pEraserData && pEraserData->ui)
			{
				CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
				if (col)
				{
					if(dwValue > col->GetTag()) //多个线程更新当前进度，保留进度最大的那个。
						col->SetTag(dwValue);
					if (col->GetTag() >= 100)
						col->SetTag(0);
					col->NeedUpdate();
				}
				break;
			}
		}
		break;
	case eto_finished:
		DeleteErasuredFile(m_ErasureFile.GetFiles());
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
	m_EreaserThreads.SetEreaureFiles(m_ErasureFile.GetFiles());
	m_EreaserThreads.StartEreasure(10);
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::AddFileUI(CVirtualFile* pFile, CLdArray<TCHAR*>* pColumeData)
{
	PFILE_ERASURE_DATA p = (PFILE_ERASURE_DATA)pFile->GetTag();
	
	if(pColumeData)
		p->ui = (CControlUI*)AddRecord(pColumeData);
	else
		p->ui = AddFile(pFile->GetFullName());

	CControlUI* col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
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
	col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
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
				s.Format(_T("包含数%d个文件（文件夹），合计%s"), p->nCount, strSize);
				desc->SetText(s);
			}
			else
			{
				CLdArray<CVirtualFile*>* streams = pFile->GetFiles();
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
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

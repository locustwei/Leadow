#include "stdafx.h"
#include "ErasureFileUI.h"
#include "..\ErasureConfig.h"

CErasureFileUI::CErasureFileUI() 
	: m_Comm(nullptr),
	m_ErasureFile()
{
	btnOpenFile = nullptr;
	btnOk = nullptr;
	lstFile = nullptr;
	m_Abort = false;
	m_Name = _T("ErasureFileUI");
	m_ItemSkin = _T("erasure/listitem_file.xml");
	m_Comm = new CLdCommunication(this, COMM_PIPE_NAME);
	m_ErasureFile.ObjectFreeMethod = CLdMethodDelegate::MakeDelegate(&ArrayDeleteObjectMethod<CLdString*>);
}

CErasureFileUI::~CErasureFileUI()
{
	if (m_Comm)
		delete m_Comm;
}

void CErasureFileUI::ExecuteFileAnalysis(CLdArray<TCHAR*>* files)
{
	DebugOutput(L"ExecuteFileAnalysis");

	if (!m_Comm->IsConnected())
		if (m_Comm->LoadHost(IMPL_PLUGIN_ID) != 0)
			return;
	CDynObject param;
	param.AddObjectAttribute(EPN_OP_REMOVEDIR, ErasureConfig.IsRemoveFolder());
	param.AddObjectAttribute(EPN_OP_METHOD, ErasureConfig.GetFileErasureMothed());

	for (int i = 0; i < files->GetCount(); i++)
	{
		param.AddArrayValue(EPN_FILES, files->Get(i));
	}

	m_Comm->CallMethod(eci_anafiles, param);

}

//���е�������
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

//��ʼ������
void CErasureFileUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOpenFile = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("openfile"), 0));
	btnOk = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));

	//��ϵͳĿ¼Ϊģ��,���Ĭ����.
	TCHAR Path[MAX_PATH] = { 0 };
	GetSystemDirectory(Path, MAX_PATH);
	CSHFolders::EnumFolderColumes(Path, this, 0);

	AddLstViewHeader(8);
}
//������ɺ��m_ErasureFile��ɾ��
void CErasureFileUI::DeleteErasuredFile(CVirtualFile* pFile)
{
	if (!pFile)
		return;
	if (pFile->GetFileType() == vft_folder)
		FreeEraseFiles(((CFolderInfo*)pFile)->GetFiles());
	if (pFile->GetTag() != 0)
		delete (PFILE_EX_DATA)pFile->GetTag();
	delete pFile;
}
//�������̸����ļ�����״̬��Ϣ
void CErasureFileUI::UpdateEraseProgressMsg(PFILE_EX_DATA pData, bool bRoot)
{
	if (!pData)
		return;

	CDuiString str;
	//int Percent = pData->nErased / pData->nFileCount * 100;

	CControlUI* col = pData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
	if (col)
	{				
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			if (!bRoot)
			{
				str.Format(_T("�Ѳ���%d���ļ�"), pData->nErased);
			}
			else
			{
				str = _T("�����");
				desc->SetBkColor(0xFF00FFFF);
			}

			if (pData->nError > 0)
			{
				CDuiString s;
				s.Format(_T("\n%d���ļ���������"), pData->nError);
				str += s;
				desc->SetBkColor(0xFFFF0000);
			}

			desc->SetText(str);
		}
		//col->SetTag(Percent);
		col->NeedUpdate();
	}
}


//�������̻ص�����
bool CErasureFileUI::EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue)
{
	CVirtualFile* pFile;	
	PFILE_EX_DATA pData;
	CControlUI* col;

	switch (op)
	{
	case eto_start:  //�ܽ��ȿ�ʼ
		btnOk->SetTag(1);
		btnOk->SetText(L"ȡ��");
		break;
	case eto_begin:  
		break;
	case eto_completed: //�����ļ�������� //���ò���״̬
		//pFile = m_ErasureFile.Find(FileName, true, true);
		pData = GetFileData(pFile);

		if (!pFile || pData == nullptr)
			break;
		pData->nErased++;
		//UpdateEraseProgressMsg(pData, pFile->GetFolder()==&m_ErasureFile);
		DeleteErasuredFile(pFile);
		break;
	case eto_progress: //�����ļ�����
		//pFile = m_ErasureFile.Find(FileName, false, true);
		pData = GetFileData(pFile);

		if (pData==nullptr)
			break;

		col = pData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
		if (col)
		{
			col->SetTag(dwValue);
			col->NeedUpdate();
		}
		break;
	case eto_error:
		//pFile = m_ErasureFile.Find(FileName, true, true);
		pData = GetFileData(pFile);
		if (pData)
		{
			pData->nErased++;
			pData->nError++;
		}
		break;
	case eto_finished:
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"ִ��");
		break;
	default:
		break;
	}
	return !m_Abort;
}

void CErasureFileUI::StatErase()
{
	if (m_ErasureFile.GetCount() == 0)
		return;

	m_Abort = false;
	DebugOutput(L"StatErase");

	if (!m_Comm->IsConnected())
		if (m_Comm->LoadHost(IMPL_PLUGIN_ID) != 0)
			return;
	CDynObject param;
	param.AddObjectAttribute(EPN_OP_REMOVEDIR, ErasureConfig.IsRemoveFolder());
	param.AddObjectAttribute(EPN_OP_METHOD, ErasureConfig.GetFileErasureMothed());

	for (int i = 0; i < m_ErasureFile.GetCount(); i++)
	{
		param.AddArrayValue(EPN_FILES, m_ErasureFile[i]->GetData());
	}

	m_Comm->CallMethod(eci_erasefiles, param);

}

bool CErasureFileUI::OnCreate()
{
	return true;
}

void CErasureFileUI::OnTerminate(DWORD exitcode)
{
}

void CErasureFileUI::OnCommand(WORD id, CDynObject& Param)
{
	switch (id)
	{
	case eci_anafiles:
		for (int i = 0; i < Param.GetArrayCount(EPN_FILES); i++)
		{
			AddFileUI(Param.GetDynObject(EPN_FILES, i));
		}
		break;
	case eci_filestatus:
		break;
	default:
		break;
	}
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

bool CErasureFileUI::IsSelecteFile(TCHAR* filename)
{
	for (int i = 0; i < m_ErasureFile.GetCount(); i++)
	{
		if (*m_ErasureFile[i] == filename)
			return true;
	}
	return false;
}

void CErasureFileUI::AddFileUI(CDynObject FileObj)
{
	//PFILE_ERASURE_DATA p = (PFILE_ERASURE_DATA)pFile->GetTag();
	CLdString* filename = new CLdString(FileObj.GetString(_T("name")));

	m_ErasureFile.Add(filename);

	CControlUI* ui = AddFile(filename->GetData());
	//SetFolderFilesData(pFile, ui);

	CControlUI* col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
	if (col)
	{
		col->SetTag(0);
		col->OnAfterPaint += MakeDelegate(this, &CErasureFileUI::OnAfterColumePaint);
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			CLdString s;
			CFileUtils::ExtractFilePath(filename->GetData(), s);
			desc->SetText(s);
		}
	}

	col = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
	if (col)
	{
		col->SetTag(0);
		//col->OnAfterPaint += MakeDelegate(this, &CErasureFileUI::OnAfterColumePaint);
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			CLdString s, strSize;
			UINT64 filesize = FileObj.GetInteger(_T("TotalSize"));
			CFileUtils::FormatFileSize(filesize, strSize);
			int filecount = FileObj.GetInteger(_T("FileCount"));

			if (filecount)
			{
				s.Format(_T("������%d���ļ����ļ��У����ϼ�%s"), filecount, strSize);
			}
			//else
			//	s.Format(_T("�ļ���С%s"), strSize);

			int adscount = FileObj.GetInteger(_T("ADSCount"));

			if (adscount)
			{
				INT64 adssize = FileObj.GetInteger(_T("ADSSizie"));

				CFileUtils::FormatFileSize(adssize, strSize);
				CLdString s1;
				s1.Format(_T("\n�ļ���%d���������������ϼ�%s"), adssize, strSize);
				s += s1;
			};

			desc->SetText(s);
		}
	}
}
//����Ҹ�·��Ϊģ�壬ȡ����Դ������������Ϣ
bool CErasureFileUI::GetViewHeader()
{
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
			CLdArray<TCHAR*> filenames;
			for(int i=0; i<dlg.GetFileCount();i++)
			{
				if (IsSelecteFile(dlg.GetFileName(i)))
					continue;
				
				filenames.Add(dlg.GetFileName(i));
			}
			
			ExecuteFileAnalysis(&filenames);

		};
	}
	else if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag())
			StatErase();
		else
		{//����û�н�����ȡ����ǰ����
			//m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
			m_Abort = true;
		}
	}
}

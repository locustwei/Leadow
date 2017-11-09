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
	m_Abort = false;
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
	for (int i = 0; i<files->GetCount(); i++)
	{
		CVirtualFile * file = files->Get(i);
		if (file)
		{
			if (file->GetFileType() == vft_folder)
				FreeEraseFiles(((CFolderInfo*)file)->GetFiles());

			if (file->GetTag() != 0)
				delete (PFILE_EX_DATA)file->GetTag();
			file->SetTag(0);
			delete file;
		}

	}
}
//�����ļ���Ŀ¼ָ�򣬲���ʱ���������ļ��еĽ���
DWORD CErasureFileUI::SetFolderFilesData(CVirtualFile* pFile, CControlUI* ui)
{
	PFILE_EX_DATA pData = new FILE_EX_DATA;
	ZeroMemory(pData, sizeof(FILE_EX_DATA));
	pData->ui = ui;
	if (pFile->GetFileType() == vft_folder)
		pData->nFileCount = ((CFolderInfo*)pFile)->GetFilesCount(true);
	else
		pData->nFileCount = 1;

	pFile->SetTag((UINT_PTR)pData);
	return 0;
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
//���һ���������ļ�
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

	m_ErasureFile.AddFile(info);
	return info;
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
//���Ҳ����ļ������ļ��У��ļ���ͳ����Ϣ
CErasureFileUI::PFILE_EX_DATA CErasureFileUI::GetFileData(CVirtualFile* pFile)
{
	if (!pFile)
		return nullptr;
	while (pFile->GetFolder() != &m_ErasureFile && pFile->GetFolder() != nullptr)
		pFile = pFile->GetFolder();
	return (PFILE_EX_DATA)pFile->GetTag();
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
		pFile = m_ErasureFile.Find(FileName, true, true);
		pData = GetFileData(pFile);

		if (!pFile || pData == nullptr)
			break;
		pData->nErased++;
		UpdateEraseProgressMsg(pData, pFile->GetFolder()==&m_ErasureFile);
		DeleteErasuredFile(pFile);
		break;
	case eto_progress: //�����ļ�����
		pFile = m_ErasureFile.Find(FileName, false, true);
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
		pFile = m_ErasureFile.Find(FileName, true, true);
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
	if (m_ErasureFile.GetFilesCount() == 0)
		return;
	m_Abort = false;
	CLdArray<TCHAR*> files;
	for(UINT i=0; i<m_ErasureFile.GetFilesCount(); i++)
	{
		files.Add(m_ErasureFile.GetFiles()->Get(i)->GetFullName());
	}
	ExecuteFileErase(this, &files);
}

DUI_BEGIN_MESSAGE_MAP(CErasureFileUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

void CErasureFileUI::AddFileUI(CVirtualFile* pFile)
{
	//PFILE_ERASURE_DATA p = (PFILE_ERASURE_DATA)pFile->GetTag();
	CControlUI* ui = AddFile(pFile->GetFullName());
	SetFolderFilesData(pFile, ui);

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
		//col->OnAfterPaint += MakeDelegate(this, &CErasureFileUI::OnAfterColumePaint);
		CControlUI* desc = col->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (desc)
		{
			CLdString s, strSize;
			if (pFile->GetFileType() == vft_folder)
			{
				CFileUtils::FormatFileSize(pFile->GetDataSize(), strSize);
				s.Format(_T("������%d���ļ����ļ��У����ϼ�%s"), ((CFolderInfo*)pFile)->GetFilesCount(true), strSize);
				//desc->SetText(s);
			}
			CLdArray<CVirtualFile*>* streams = ((CFileInfo*)pFile)->GetADStreams();
			if (streams && streams->GetCount()>0)
			{
				INT64 nSize = 0;
				for(int i=0; i<streams->GetCount();i++)
				{
					nSize += streams->Get(i)->GetDataSize();
				}
				CFileUtils::FormatFileSize(nSize, strSize);
				CLdString s1;
				s1.Format(_T("\n�ļ���%d���������������ϼ�%s"), streams->GetCount(), strSize);
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
		{//����û�н�����ȡ����ǰ����
			//m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
			m_Abort = true;
		}
	}
}

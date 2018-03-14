#include "stdafx.h"
#include "ErasureVolumeUI.h"
#include "EraserUI.h"
//#include "../LdFileEraser/ErasureLibrary.h"

//#include "../eraser/VolumeEx.h"

CErasureVolumeUI::CErasureVolumeUI():
	m_Volumes()
	//,m_EreaserThreads(this)
{
	btnOk = nullptr;
	m_Name = _T("ErasureVolumeUI");
	m_ItemSkin = _T("erasure/listitem_volume.xml");            //
}


CErasureVolumeUI::~CErasureVolumeUI()
{
	//m_EreaserThreads.StopThreads();
}

void CErasureVolumeUI::UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent, DWORD time)
{
	CControlUI* ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
	CDuiString str;
//	if(pData->nStatus == efs_error)
//	{
//		str.Format(_T("�������� �������%x"), pData->nErrorCode);
//		ChildUI->SetBkColor(0xFFFF0000);
//	}
//	else
//	{
//		DWORD t = (GetTickCount() - time) / 1000;
//		if (Percent < 100)
//			str.Format(_T("�����%d%% ��ʱ%d�룬����Ҫ%d��"), Percent, t, t * (100 - Percent) / Percent);
//		else
//		{
//			str.Format(_T("����� ��ʱ%d��"), t);
//			ChildUI->SetBkColor(0xFF00FFFF);
//		}
//	}
	ChildUI->SetText(str);
	ui->SetTag(Percent);
	ui->NeedUpdate();
}

void CErasureVolumeUI::ShowAnalysisResult(CVolumeInfo* pVolume, CControlUI* ui)
{
	CControlUI* ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
//	CControlUI* Desc;
//	CGifAnimUI* Gif;
	PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)pVolume->GetTag();

	CDuiString str;
//	if (pData->nStatus == efs_error)
//	{
//		str.Format(_T("�������� �������%x"), pData->nErrorCode);
//	}
//
//	if(ChildUI)
//	{
//		Desc = ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
//		if (pData->nStatus != efs_error)
//		{
////			str.Format(_T("�����ļ���%lld, Ŀ¼��%lld��"),
////				pVolume->GetFileCount(),
////				pVolume->GetDirectoryCount());
//		}
//		Desc->SetText(str);
//		Gif = (CGifAnimUI*)ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
//		if (Gif)
//		{
//			Gif->StopGif();
//			Gif->SetVisible(false);
//		}
//	}
/*

	ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume3"), 0);
	if(ChildUI)
	{
		Desc = ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (pData->nStatus != efs_error)
		{
//			str.Format(_T("��ɾ�����ļ���%lld�����пɱ��ָ���%lld�������ļ�ɾ���ۼ�Ԥ����Ҫ%d��"),
//				pVolume->GetTrackCount(),
//				pVolume->GetRemoveableCount(),
//				pVolume->GetTrackCount() / 100 * (pVolume->GetCrateSpeed() + pVolume->GetDelSpeed()) / 1000);
		}
		Desc->SetText(str);
		Gif = (CGifAnimUI*)ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
		if (Gif)
		{
			Gif->StopGif();
			Gif->SetVisible(false);
		}
	}

	ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume4"), 0);
	if (ChildUI)
	{
		Desc = ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
		if (pData->nStatus != efs_error)
		{
//			str.Format(_T("���̶�д�ٶ�%dms/M, �������̿��пռ�Ԥ����Ҫ%d��"),
//				pVolume->GetWriteSpeed() / 10,
//				pVolume->GetAvailableFreeSpace() / 0xA00000 * pVolume->GetWriteSpeed() / 1000);
		}
		Desc->SetText(str);
		Gif = (CGifAnimUI*)ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
		if (Gif)
		{
			Gif->StopGif();
			Gif->SetVisible(false);
		}
	}
*/

	ui->SetFixedHeight(80);
}

bool CErasureVolumeUI::OnAfterColumePaint(PVOID Param)
{
	PUI_PAINT_PARAM pPaint = (PUI_PAINT_PARAM)Param;
	UINT percent = (UINT)pPaint->sender->GetTag();
	if (percent == 0)
		return true;
	RECT rect = pPaint->sender->GetPos();
	rect.bottom = rect.top + 20;
	rect.right = rect.left + ((rect.right - rect.left) * percent) / 100 ;
	CRenderEngine::DrawColor(pPaint->hDc, rect, 0x80FFFF00);
	return true;
}

DUI_BEGIN_MESSAGE_MAP(CErasureVolumeUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

//ö���̷��ݴ����ڲ���ÿ�����µĻ���վ�ļ�
BOOL CErasureVolumeUI::EnumVolume_Callback(PVOID data, UINT_PTR Param)
{
	TCHAR* pData = (TCHAR*)data;

	CVolumeInfo* volume = new CVolumeInfo();
	volume->SetFileName(pData);
	m_Volumes.Add(volume);
	return TRUE;
}

bool CErasureVolumeUI::EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue)
{
	PFILE_ERASURE_DATA pEraserData;
	CVirtualFile* pFile = nullptr;
	switch (op)
	{
	case eto_start:  //�ܽ��ȿ�ʼ
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_analy:
		break;
	case eto_analied:
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue != 0)
		{
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
		}
		
		ShowAnalysisResult((CVolumeInfo*)pFile, pEraserData->ui);

		break;
	case eto_begin:  //�����ļ���ʼ
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CCheckBoxUI* checkbox = (CCheckBoxUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByClassProc, DUI_CTR_CHECKBOX, 0);
			if (!checkbox || !checkbox->GetCheck())
				return false;
		}
		break;
	case eto_completed: //�����ļ��������
						//���ò���״̬
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
		{
			pEraserData->nStatus = efs_erasured;
		}
		else
		{
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
		}
		break;
	case eto_progress: //�����ļ�����
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
			if (col)
			{
				col->SetTag(dwValue % 100);
				col->NeedUpdate();
			}
		}
		break;
	case eto_finished:
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"OK");
		break;
	case eto_freespace: 
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
			pEraserData->FreespaceTime = GetTickCount(); //��ʼ����ʱ��
		else
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume4"), 0);
			UpdateEraseProgressMsg(pEraserData, col, dwValue, pEraserData->FreespaceTime);
		}
		break;
	case eto_track: 
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
			pEraserData->TrackTime = GetTickCount();  //��ʼ����ʱ��
		else
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume3"), 0);
			UpdateEraseProgressMsg(pEraserData, col, dwValue, pEraserData->TrackTime);  
		}
		break;
	default: 
		break;
	}
	return true;
}

bool CErasureVolumeUI::GetViewHeader()
{
	return CSHFolders::EnumFolderColumes(CSIDL_DRIVES, this, 0) == 0;
}

bool CErasureVolumeUI::AnalyResult(TCHAR* FileName, PVOID pData)
{
	return false;
}

void CErasureVolumeUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOk = (CButtonUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0);
	CVolumeUtils::MountedVolumes(CMethodDelegate::MakeDelegate(this, &CErasureVolumeUI::EnumVolume_Callback), 0);
	CLdArray<TCHAR*> atts;

	
	for (int i = m_Volumes.GetCount() - 1; i>=0; i--)
	{
		CVolumeInfo* volume = (CVolumeInfo*)m_Volumes.Get(i);
		DWORD dwError;
		VOLUME_FILE_SYSTEM fs = volume->GetFileSystem(&dwError);
		//�жϴ����Ƿ����
		if (dwError != 0 || fs == FS_UNKNOW)
		{
			m_Volumes.Delete(i);
			continue;
		}

		CSHFolders::GetFileAttributeValue(volume->GetFullName(), &atts);
		atts.Insert(0, nullptr);

		PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
		ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
		p->ui = AddRecord(&atts);
		p->ui->SetTag((UINT_PTR)volume);
		CControlUI* col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
		if(col)
		{
			col->SetTag(0);
			col->OnAfterPaint += MakeDelegate(this, &CErasureVolumeUI::OnAfterColumePaint);
		}
		col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume4"), 0);
		if (col)
		{
			col->SetTag(0);
			col->OnAfterPaint += MakeDelegate(this, &CErasureVolumeUI::OnAfterColumePaint);
		}
		col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume3"), 0);
		if (col)
		{
			col->SetTag(0);
			col->OnAfterPaint += MakeDelegate(this, &CErasureVolumeUI::OnAfterColumePaint);
		}
		volume->SetTag((UINT_PTR)p);

		for (int j = 1; j < atts.GetCount(); j++)
			delete atts[j];
		atts.Clear();
	}
	//AddFolder(CSIDL_DRIVES);
	StatAnalysis();
}


void CErasureVolumeUI::StatAnalysis()
{
	CLdArray<TCHAR*> files;
	for(int i=0; i<m_Volumes.GetCount();i++)
	{
		files.Add(m_Volumes.Get(i)->GetFullName());
	}
	//ExecuteVolumeAnalysis(this, &files);
}

void CErasureVolumeUI::StatErase()
{
	CLdArray<TCHAR*> files;
	for (int i = 0; i<m_Volumes.GetCount(); i++)
	{
		files.Add(m_Volumes.Get(i)->GetFullName());
	}
	//ExecuteFileErase(this, &files);
}

void CErasureVolumeUI::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag())
			StatErase();
		else
		{//����û�н�����ȡ����ǰ����
			//m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

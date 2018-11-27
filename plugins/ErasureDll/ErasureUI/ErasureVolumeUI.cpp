#include "stdafx.h"
#include "ErasureVolumeUI.h"
#include "..\ErasureConfig.h"
//#include "../LdFileEraser/ErasureLibrary.h"

//#include "../eraser/VolumeEx.h"

CErasureVolumeUI::CErasureVolumeUI():
	m_Volumes()
	//,m_EreaserThreads(this)
{
	btnOk = nullptr;
	m_Name = _T("ErasureVolumeUI");
	m_ItemSkin = _T("erasure/listitem_volume.xml");            //
	m_Volumes.ObjectFreeMethod = CLdMethodDelegate::MakeDelegate(ArrayDeleteObjectMethod<CEraseVolumeData*>);
	m_Comm = new CLdCommunication(this, COMM_PIPE_NAME);

}


CErasureVolumeUI::~CErasureVolumeUI()
{
	if (m_Comm)
		delete m_Comm;
}

bool CErasureVolumeUI::OnCreate()
{
	return true;
}

void CErasureVolumeUI::OnTerminate(DWORD exitcode)
{
}

void CErasureVolumeUI::OnCommand(WORD id, CDynObject& Param)
{
	switch (id)
	{
	case eci_anafiles:
		//ListFiles(Param);
		break;
	case eci_filestatus:
		//OnEraseFileStatus(Param);
		break;
	default:
		break;
	}
}

//void CErasureVolumeUI::UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui, int Percent, DWORD time)
//{
//	CControlUI* ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
//	CDuiString str;
//
//	ChildUI->SetText(str);
//	ui->SetTag(Percent);
//	ui->NeedUpdate();
//}

void CErasureVolumeUI::ShowAnalysisResult(TCHAR* Volume, CControlUI* ui)
{
	CControlUI* ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
//	CControlUI* Desc;
//	CGifAnimUI* Gif;
	//PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)pVolume->GetTag();

	//CDuiString str;
//	if (pData->nStatus == efs_error)
//	{
//		str.Format(_T("发生错误， 错误代码%x"), pData->nErrorCode);
//	}
//
//	if(ChildUI)
//	{
//		Desc = ChildUI->FindControl(CDuiUtils::FindControlByNameProc, _T("desc"), 0);
//		if (pData->nStatus != efs_error)
//		{
////			str.Format(_T("磁盘文件数%lld, 目录数%lld。"),
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
//			str.Format(_T("被删除的文件数%lld，其中可被恢复数%lld。擦除文件删除痕迹预计需要%d秒"),
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
//			str.Format(_T("磁盘读写速度%dms/M, 擦除磁盘空闲空间预计需要%d秒"),
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

//枚举盘符

void CErasureVolumeUI::AddDiskVolumes()
{
	CLdArray<CLdString*> Volumes;
	Volumes.ObjectFreeMethod = CLdMethodDelegate::MakeDelegate(&ArrayDeleteObjectMethod<CLdString*>);

	CVolumeUtils::EnumVolumeNames(CLdMethodDelegate::MakeDelegate(this, &CErasureVolumeUI::EnumVolume_Callback), (UINT_PTR)&Volumes);

	for (int i = Volumes.GetCount() - 1; i >= 0; i--)
	{
		CVolumeInfo volume(Volumes[i]->GetData());
		if (volume.GetVolumePath() == nullptr)
			continue;
		DWORD dwError;
		VOLUME_FILE_SYSTEM fs = volume.GetFileSystem(&dwError);
		if (dwError != 0)
			continue;

		CLdString s4;
		switch (fs)
		{
		case LeadowDisk::FILESYSTEM_TYPE_NTFS:
			s4 = _T("NTFS");
			break;
		case LeadowDisk::FILESYSTEM_TYPE_FAT:
			s4 = _T("FAT32");
			break;
		case LeadowDisk::FILESYSTEM_TYPE_EXFAT:
			s4 = _T("EXFAT");
			break;
		default:
			continue;
		}

		CLdString s1;
		UINT type = GetDriveType(volume.GetVolumeGuid());
		switch (type)
		{
		case DRIVE_REMOVABLE:
			s1 = _T("可移动磁盘");
			break;
		case DRIVE_FIXED:
			s1 = _T("本地磁盘");
			break;
		default:
			continue;
		}

		CLdArray<TCHAR*> atts;

		CLdString s = volume.GetShlDisplayName();
		CLdString s2;
		CFileUtils::FormatFileSize(volume.GetTotalSize(nullptr), s2);
		CLdString s3;
		CFileUtils::FormatFileSize(volume.GetAvailableFreeSpace(nullptr), s3);
		atts.Add(nullptr);
		atts.Add(s);
		atts.Add(s1);
		atts.Add(s2);
		atts.Add(s3);
		atts.Add(s4);

		CEraseVolumeData* p = new CEraseVolumeData;
		p->volume = new CLdString(volume.GetVolumePath());
		p->ui = AddRecord(&atts);
		//p->ui->SetTag((UINT_PTR)volume);
		CControlUI* col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
		if (col)
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
		m_Volumes.Add(p);
	}

}

BOOL CErasureVolumeUI::EnumVolume_Callback(PVOID data, UINT_PTR Param)
{
	TCHAR* pData = (TCHAR*)data;

	CLdArray<CLdString*>* pVolumes = (CLdArray<CLdString*>*)Param;
	pVolumes->Add(new CLdString(pData));
	return TRUE;
}

//bool CErasureVolumeUI::EraserReprotStatus(TCHAR* FileName, E_THREAD_OPTION op, DWORD dwValue)
//{
//	PFILE_ERASURE_DATA pEraserData;
//	CVirtualFile* pFile = nullptr;
//	switch (op)
//	{
//	case eto_start:  //总进度开始
//		btnOk->SetTag(1);
//		btnOk->SetText(L"Cancel");
//		break;
//	case eto_analy:
//		break;
//	case eto_analied:
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (dwValue != 0)
//		{
//			pEraserData->nStatus = efs_error;
//			pEraserData->nErrorCode = dwValue;
//		}
//		
//		//ShowAnalysisResult((CVolumeInfo*)pFile, pEraserData->ui);
//
//		break;
//	case eto_begin:  //单个文件开始
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (pEraserData && pEraserData->ui)
//		{
//			CCheckBoxUI* checkbox = (CCheckBoxUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByClassProc, DUI_CTR_CHECKBOX, 0);
//			if (!checkbox || !checkbox->GetCheck())
//				return false;
//		}
//		break;
//	case eto_completed: //单个文件擦除完成
//						//设置擦除状态
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (dwValue == 0)
//		{
//			pEraserData->nStatus = efs_erasured;
//		}
//		else
//		{
//			pEraserData->nStatus = efs_error;
//			pEraserData->nErrorCode = dwValue;
//		}
//		break;
//	case eto_progress: //单个文件进度
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (pEraserData && pEraserData->ui)
//		{
//			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
//			if (col)
//			{
//				col->SetTag(dwValue % 100);
//				col->NeedUpdate();
//			}
//		}
//		break;
//	case eto_finished:
//		btnOk->SetTag(0);
//		btnOk->SetEnabled(true);
//		btnOk->SetText(L"OK");
//		break;
//	case eto_freespace: 
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (dwValue == 0)
//			pEraserData->FreespaceTime = GetTickCount(); //开始擦除时间
//		else
//		{
//			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume4"), 0);
//			UpdateEraseProgressMsg(pEraserData, col, dwValue, pEraserData->FreespaceTime);
//		}
//		break;
//	case eto_track: 
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (dwValue == 0)
//			pEraserData->TrackTime = GetTickCount();  //开始擦除时间
//		else
//		{
//			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume3"), 0);
//			UpdateEraseProgressMsg(pEraserData, col, dwValue, pEraserData->TrackTime);  
//		}
//		break;
//	default: 
//		break;
//	}
//	return true;
//}

bool CErasureVolumeUI::GetViewHeader()
{
	PSH_HEAD_INFO p = new SH_HEAD_INFO;
	p->cxChar = 20;
	p->fmt = 0;
	CLdString s = _T("名称");
	p->szName = new TCHAR[s.GetLength() + 1];
	s.CopyTo(p->szName);
	m_Columes.Add(p);

	p = new SH_HEAD_INFO;
	p->cxChar = 25;
	p->fmt = 0;
	s = _T("类型");
	p->szName = new TCHAR[s.GetLength() + 1];
	s.CopyTo(p->szName);
	m_Columes.Add(p);

	p = new SH_HEAD_INFO;
	p->cxChar = 16;
	p->fmt = 1;
	s = _T("总大小");
	p->szName = new TCHAR[s.GetLength() + 1];
	s.CopyTo(p->szName);
	m_Columes.Add(p);

	p = new SH_HEAD_INFO;
	p->cxChar = 16;
	p->fmt = 1;
	s = _T("可用空间");
	p->szName = new TCHAR[s.GetLength() + 1];
	s.CopyTo(p->szName);
	m_Columes.Add(p);

	p = new SH_HEAD_INFO;
	p->cxChar = 15;
	p->fmt = 0;
	s = _T("文件系统");
	p->szName = new TCHAR[s.GetLength() + 1];
	s.CopyTo(p->szName);
	m_Columes.Add(p);

	return true;
}

bool CErasureVolumeUI::AnalyResult(TCHAR* FileName, PVOID pData)
{
	return false;
}

void CErasureVolumeUI::AttanchControl(CControlUI* pCtrl)
{
	
	__super::AttanchControl(pCtrl);
	btnOk = (CButtonUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0);

	AddDiskVolumes();

	
	StatAnalysis();
}


void CErasureVolumeUI::StatAnalysis()
{
	
	if (!m_Comm->IsConnected())
		if (m_Comm->LoadHost(IMPL_PLUGIN_ID) != 0)
			return;

	CDynObject param;
	param.AddObjectAttribute(EPN_OP_REMOVEDIR, ErasureConfig.IsRemoveFolder());
	param.AddObjectAttribute(EPN_OP_METHOD, ErasureConfig.GetFileErasureMothed());

	for (int i = 0; i < m_Volumes.GetCount(); i++)
	{
		param.AddArrayValue(EPN_FILES, m_Volumes.Get(i)->volume->GetData());
	}

	m_Comm->CallMethod(eci_anavolume, param);
	
}

void CErasureVolumeUI::StatErase()
{
	//CLdArray<TCHAR*> files;
	//for (int i = 0; i<m_Volumes.GetCount(); i++)
	//{
	//	files.Add(m_Volumes.Get(i)->GetFullName());
	//}
	//ExecuteFileErase(this, &files);
}

void CErasureVolumeUI::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == btnOk)
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

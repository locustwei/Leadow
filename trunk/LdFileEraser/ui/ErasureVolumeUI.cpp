#include "stdafx.h"
#include "ErasureVolumeUI.h"
#include "../eraser/VolumeEx.h"

CErasureVolumeUI::CErasureVolumeUI():
	m_Volumes(),
	m_EreaserThreads(this)
{
	btnOk = nullptr;
	m_Name = _T("ErasureVolumeUI");
	m_ItemSkin = _T("erasure\\listitem_volume.xml");            //
}


CErasureVolumeUI::~CErasureVolumeUI()
{
	m_EreaserThreads.StopThreads();
}

void CErasureVolumeUI::OnSelectChanged(TNotifyUI & msg)
{
/*	CCheckBoxUI* pchk_ui = (CCheckBoxUI*)msg.pSender->GetInterface(DUI_CTR_CHECKBOX);
	if (pchk_ui)
	{
		CListContainerElementUI* p_ui = (CListContainerElementUI*)pchk_ui->FindParentControl(CDuiUtils::FindControlByClassProc, DUI_CTR_LISTCONTAINERELEMENT, 0);
		if (!p_ui)
			return;
		if(pchk_ui->GetCheck())
		{
			CVolumeEx* p_info = (CVolumeEx*)p_ui->GetTag();
			PFILE_ERASURE_DATA pEraseData = (PFILE_ERASURE_DATA)p_info->GetTag();

		}
		else
			p_ui->SetFixedHeight(40);
	}*/
}

void CErasureVolumeUI::OnItemClick(TNotifyUI & msg)
{
}

void CErasureVolumeUI::UpdateEraseProgressMsg(PFILE_ERASURE_DATA pData, CControlUI* ui)
{

}

void CErasureVolumeUI::ShowAnalysisResult(CVolumeEx* pVolume, CControlUI* ui)
{
	CControlUI* ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("freespace"), 0);
	if(ChildUI)
	{
		CDuiString str;
		str.Format(_T("磁盘读写速度%dms/M。擦除磁盘空闲空间预计需要%d秒"),
			pVolume->GetWriteSpeed(),
			pVolume->GetAvailableFreeSpace() / 0xA00000 * pVolume->GetWriteSpeed() / 1000);
		ChildUI->SetText(str);
	}

	ChildUI = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("track"), 0);
	if(ChildUI)
	{
		CDuiString str;
		str.Format(_T("被删除的文件数%lld，其中可被恢复数%lld。擦除文件删除痕迹预计需要%d秒"),
			pVolume->GetTrackCount(),
			pVolume->GetRemoveableCount(),
			pVolume->GetTrackCount() / 100 * (pVolume->GetCrateSpeed() + pVolume->GetDelSpeed()) / 1000);
		ChildUI->SetText(str);
	}

	CGifAnimUI* Gif = (CGifAnimUI*)ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress1"), 0);
	if (Gif)
	{
		Gif->StopGif();
		Gif->SetVisible(false);
	}
	Gif = (CGifAnimUI*)ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress2"), 0);
	if (Gif)
	{
		Gif->StopGif();
		Gif->SetVisible(false);
	}
	ui->SetFixedHeight(80);
}

bool CErasureVolumeUI::OnAfterColumePaint(PVOID Param)
{
	PUI_PAINT_PARAM pPaint = (PUI_PAINT_PARAM)Param;
	UINT percent = (UINT)pPaint->sender->GetTag();
	if (percent == 0)
		return true;
	RECT rect = pPaint->sender->GetPos();
	rect.right = rect.left + (rect.right - rect.left) * (percent / 100);
	CRenderEngine::DrawColor(pPaint->hDc, rect, 0x80FFFF00);
	return true;
}

DUI_BEGIN_MESSAGE_MAP(CErasureVolumeUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//枚举盘符暂存用于查找每个盘下的回收站文件
BOOL CErasureVolumeUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CVolumeEx* volume = new CVolumeEx();
	volume->SetFileName(pData);
	m_Volumes.Add(volume);
	return TRUE;
}

bool CErasureVolumeUI::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	PFILE_ERASURE_DATA pEraserData;

	switch (op)
	{
	case eto_start:  //总进度开始
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_analy:
		break;
	case eto_analied:
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
		{
			//磁盘分析结束，分析数据在列表中显示
			ShowAnalysisResult((CVolumeEx*)pFile, pEraserData->ui);

			if(pEraserData->ui)
				pEraserData->ui->SetFixedHeight(80);
		}
		else
		{
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
		}
		break;
	case eto_begin:  //单个文件开始
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CCheckBoxUI* checkbox = (CCheckBoxUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByClassProc, DUI_CTR_CHECKBOX, 0);
			if (!checkbox || !checkbox->GetCheck())
				return false;
		}
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
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
		}
		break;
	case eto_progress: //单个文件进度
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume1"), 0);
			if (col)
			{
				col->SetTag(dwValue);
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
		if (pEraserData && pEraserData->ui)
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
			if (col)
			{
				col->SetTag(dwValue);
				col->NeedUpdate();
			}
		}
		break;
	case eto_track: 
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CControlUI* col = pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume3"), 0);
			if (col)
			{
				col->SetTag(dwValue);
				col->NeedUpdate();
			}
		}
		break;
	default: 
		break;
	}
	return true;
};

void CErasureVolumeUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOk = (CButtonUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0);
	CVolumeUtils::MountedVolumes(this, 0);
	CLdArray<TCHAR*> atts;

	CSHFolders::EnumFolderColumes(CSIDL_DRIVES, this, 0);
	for (int i = m_Volumes.GetCount() - 1; i>=0; i--)
	{
		CVolumeEx* volume = (CVolumeEx*)m_Volumes.Get(i);
		DWORD dwError;
		VOLUME_FILE_SYSTEM fs = volume->GetFileSystem(&dwError);
		//判断磁盘是否可用
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
		col = p->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("colume2"), 0);
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
	m_EreaserThreads.SetEreaureFiles(&m_Volumes);
	m_EreaserThreads.StartAnalysis(4);
}

void CErasureVolumeUI::StatErase()
{
	m_EreaserThreads.SetEreaureMethod(&CErasureMethod::Pseudorandom());
	m_EreaserThreads.SetEreaureFiles(&m_Volumes);
	m_EreaserThreads.StartEreasure(4);
}

void CErasureVolumeUI::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == btnOk)
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

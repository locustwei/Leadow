#include "stdafx.h"
#include "ErasureVolumeUI.h"
#include <thread>

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
	CCheckBoxUI* pchk_ui = (CCheckBoxUI*)msg.pSender->GetInterface(DUI_CTR_CHECKBOX);
	if (pchk_ui)
	{
		CListContainerElementUI* p_ui = (CListContainerElementUI*)pchk_ui->FindParentControl(CDuiUtils::FindControlByClassProc, DUI_CTR_LISTCONTAINERELEMENT, 0);
		if (!p_ui)
			return;
		if(pchk_ui->GetCheck())
		{
			CVolumeInfo* p_info = (CVolumeInfo*)p_ui->GetTag();
			PFILE_ERASURE_DATA pEraseData = (PFILE_ERASURE_DATA)p_info->GetTag();
			if (pEraseData->pAnalyData)
				p_ui->SetFixedHeight(80);
			else
				;// m_EreaserThreads.AnalyVolume(p_info);
		}
		else
			p_ui->SetFixedHeight(40);
	}
}

void CErasureVolumeUI::OnItemClick(TNotifyUI & msg)
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureVolumeUI, CShFileViewUI)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//枚举盘符暂存用于查找每个盘下的回收站文件
BOOL CErasureVolumeUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CVolumeInfo* volume = new CVolumeInfo();
	volume->SetFileName(pData);
	m_Volumes.Add(volume);
	return TRUE;
}

bool CErasureVolumeUI::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	PFILE_ERASURE_DATA pEraserData;
	CProgressUI* pui;

	switch (op)
	{
	case eto_start:  //总进度开始
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_begin:  //单个文件开始
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			CCheckBoxUI* checkbox = (CCheckBoxUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByClassProc, DUI_CTR_CHECKBOX, 0);
			if (!checkbox || !checkbox->GetCheck())
				return false;

			pui = (CProgressUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
			if (pui)
			{
				pui->SetVisible(true);
			}
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
			pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
			pEraserData->nStatus = efs_error;
			pEraserData->nErrorCode = dwValue;
		}
		break;
	case eto_progress: //单个文件进度
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (pEraserData && pEraserData->ui)
		{
			pui = (CProgressUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
			if (pui)
				pui->SetValue(dwValue);
		}
		break;
	case eto_finished:
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"OK");
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
		CVolumeInfo* volume = (CVolumeInfo*)m_Volumes.Get(i);
		DWORD dwError;
		VOLUME_FILE_SYSTEM fs = volume->GetFileSystem(&dwError);
		//判断磁盘是否可用
		if (dwError != 0)
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
		volume->SetTag((UINT_PTR)p);

		for (int j = 1; j < atts.GetCount(); j++)
			delete atts[j];
		atts.Clear();
	}
	//AddFolder(CSIDL_DRIVES);
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

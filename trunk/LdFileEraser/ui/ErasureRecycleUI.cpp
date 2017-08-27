#include "stdafx.h"
#include "ErasureRecycleUI.h"

/*
记录文件的隶属路径
用于统计目录下的文件被擦除进度，
*/

CErasureRecycleUI::CErasureRecycleUI():
	CErasureFileUI()
{
	//btnOk = nullptr;
	m_Name = _T("ErasureRecycleUI");
	m_ItemSkin = _T("erasure\\listitem_recycle.xml");            //
}

CErasureRecycleUI::~CErasureRecycleUI()
{
	//m_EreaserThreads.StopThreads();
	//FreeRecycleFiles(m_RecycleFile.GetFiles());
}
//
//void CErasureRecycleUI::OnSelectChanged(TNotifyUI & msg)
//{
//}
//
//void CErasureRecycleUI::OnItemClick(TNotifyUI & msg)
//{
//	
//}

DUI_BEGIN_MESSAGE_MAP(CErasureRecycleUI, CErasureFileUI)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//回收站实际文件
BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	if (_tcsicmp(pData->cFileName, _T("desktop.ini")) == 0)
		return true;

	CLdString FileName = (TCHAR*)Param;
	FileName += pData->cFileName;

	AddEraseFile(FileName);

	return true;
}

////设置文件的目录指向，擦除时更新隶属文件夹的进度
//DWORD CErasureRecycleUI::SetFolderFilesData(CLdArray<CVirtualFile*>* files)
//{
//	DWORD result = files->GetCount();
//	for (int i = 0; i < files->GetCount(); i++)
//	{
//		CVirtualFile* file = files->Get(i);
//
//		PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
//		ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
//		file->SetTag((UINT_PTR)p);
//		if (file->GetFileType() == vft_folder)
//		{
//			DWORD count = SetFolderFilesData(file->GetFiles());
//			p->nCount = count;
//			result += count;
//		}
//	}
//
//	return result;
//}

//找到虚拟文件并与实际文件对应，
BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);

	CVirtualFile* file = m_ErasureFile.Find(fi.szDisplayName);

	if (file)
	{
		AddFileUI(file, pData);
	
		//PFILE_ERASURE_DATA p = (PFILE_ERASURE_DATA)file->GetTag();

		//p->ui = AddRecord(pData);
		//p->ui->OnAfterPaint += MakeDelegate(this, &CErasureRecycleUI::OnAfterColumePaint);
	}

	return true;
}

//枚举盘符暂存用于查找每个盘下的回收站文件
BOOL CErasureRecycleUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CLdArray<CLdString*>* pVolumes = (CLdArray<CLdString*>*)Param;
	pVolumes->Add(new CLdString(pData));
	return TRUE;
}

bool CErasureRecycleUI::GetViewHeader()
{
	return CSHFolders::EnumFolderColumes(CSIDL_BITBUCKET, this, 0) == 0;
}

void CErasureRecycleUI::EnumRecyleFiels()
{
	DWORD oldMode;
	CLdArray<CLdString*> Volumes;
	CLdString sid;
	CLdString recyclePath;

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);  //防止弹出文件错误对话框
	WIN_OS_TYPE os = GetOsType();
	if (GetCurrentUserSID(sid) != 0)
		return;
	CVolumeUtils::MountedVolumes(this, (UINT_PTR)&Volumes);

	for(int i=0; i<Volumes.GetCount(); i++)
	{
		recyclePath = Volumes[i]->GetData();
		if (os >= Windows_Vista)
			recyclePath += _T("$RECYCLE.BIN");
		else
			recyclePath += _T("RECYCLE");

		switch (CVolumeUtils::GetVolumeFileSystem(Volumes[i]->GetData()))
		{
		case FS_NTFS:
			if (os < Windows_Vista)
				recyclePath += _T("R");
			recyclePath += '\\';
			recyclePath += sid;
			break;
		default:
			if (os < Windows_Vista)
				recyclePath += _T("D");
		}

		recyclePath += _T("\\");
//		m_RecycleFiles.Tag = (UINT_PTR)recyclePath.GetData();
		CFileUtils::FindFile(recyclePath, L"*.*", this, (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}
	m_ErasureFile.Sort();

	SetThreadErrorMode(oldMode, &oldMode);
}

//void CErasureRecycleUI::FreeRecycleFiles(CLdArray<CVirtualFile*>* files)
//{
//	for(int i=0;i<files->GetCount();i++)
//	{
//		CVirtualFile * file = files->Get(i);
//		if (file)
//		{
//			PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
//			if(pData)
//			{
//				delete pData;
//			}
//			file->SetTag(0);
//			if (file->GetFileType() == vft_folder)
//				FreeRecycleFiles(file->GetFiles());
//		}
//	}
//}

void CErasureRecycleUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	//回收站中的实际文件（c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//回收站的虚拟文件（原文件信息）
	AddFolder(CSIDL_BITBUCKET);
	if (lstFile->GetCount() == 0)
		AddLstViewHeader(5);
}

//void CErasureRecycleUI::DeleteErasuredFile(CLdArray<CVirtualFile*>* files)
//{
//	for(int i=files->GetCount()-1;i>=0;i--)
//	{
//		CVirtualFile* file = files->Get(i);
//		PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
//		if (pData && pData->nStatus == efs_erasured)
//		{
//			if (file->GetFileType() == vft_folder)
//				DeleteErasuredFile(file->GetFiles());
//			if (file->GetTag())
//			{
//				delete (PFILE_ERASURE_DATA)file->GetTag();
//				file->SetTag(0);
//			}
//			files->Delete(i);
//			delete file;
//		}
//	}
//}

//bool CErasureRecycleUI::OnListItemPaint(PVOID Param)
//{
//	PUI_PAINT_PARAM pPaint = (PUI_PAINT_PARAM)Param;
//	UINT percent = (UINT)pPaint->sender->GetTag();
//	if (percent == 0)
//		return true;
//	RECT rect = pPaint->sender->GetPos();
//	rect.right = rect.left + (rect.right - rect.left) / 100 * percent;
//	CRenderEngine::DrawColor(pPaint->hDc, rect, 0x80FFFF00);
//	return true;
//}

//bool CErasureRecycleUI::EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue)
//{
//	PFILE_ERASURE_DATA pEraserData;
//
//	switch(op)
//	{
//	case eto_start:  //总进度开始
//		btnOk->SetTag(1);
//		btnOk->SetText(L"Cancel");
//		break;
//	case eto_begin:  //单个文件开始
//		for (CVirtualFile* p = pFile; p!=nullptr; p=p->GetFolder())
//		{//找到所属文件夹对应listview行，显示进度条
//			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());
//			if(!pEraserData)
//			{//删除记录文件，补上Data；
//				pEraserData = new FILE_ERASURE_DATA;
//				ZeroMemory(pEraserData, sizeof(FILE_ERASURE_DATA));
//				p->SetTag((UINT_PTR)pEraserData);
//			}
//		}
//		break;
//	case eto_completed: //单个文件擦除完成
//		//设置擦除状态
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (dwValue == 0)
//		{
//			pEraserData->nStatus = efs_erasured;
//		}
//		else
//		{
//			pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//			pEraserData->nStatus = efs_error;
//			pEraserData->nErrorCode = dwValue;
//		}
//		for (CVirtualFile* p = pFile; p != nullptr; p = p->GetFolder())
//		{
//			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());
//			if (pEraserData)
//				InterlockedIncrement(&pEraserData->nErasued);
//			//更新所属文件夹进度
//			if (pEraserData && pEraserData->ui)
//			{
//				if (pEraserData->nCount == 0 || pEraserData->nErasued > pEraserData->nCount)
//					pEraserData->ui->SetTag(100);
//				//pui->SetValue(pui->GetMaxValue());
//				else
//					pEraserData->ui->SetTag(pEraserData->nErasued * 100 / pEraserData->nCount);
//					//pui->SetValue(pEraserData->nErasued * 100 / pEraserData->nCount);
//				pEraserData->ui->NeedUpdate();
//			}
//		}
//		break;
//	case eto_progress: //单个文件进度
//		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
//		if (pEraserData && pEraserData->ui)
//		{
//			pEraserData->ui->SetTag(dwValue);
//			pEraserData->ui->NeedUpdate();
//		}
//		break;
//	case eto_finished:
//		DeleteErasuredFile(m_RecycleFile.GetFiles());
//		btnOk->SetTag(0);
//		btnOk->SetEnabled(true);
//		btnOk->SetText(L"OK");
//		break;
//	}
//	return true;
//}
//
//void CErasureRecycleUI::StatErase()
//{
//	m_EreaserThreads.SetEreaureMethod(&CErasureMethod::Pseudorandom());
//	m_EreaserThreads.SetEreaureFiles(m_RecycleFile.GetFiles());
//	m_EreaserThreads.StartEreasure(10);
//}
//
//void CErasureRecycleUI::OnClick(TNotifyUI& msg)
//{
//	if (msg.pSender == btnOk)
//	{
//		if (!btnOk->GetTag())
//			StatErase();
//		else
//		{//擦除没有结束，取消当前任务
//			m_EreaserThreads.StopThreads();
//			btnOk->SetEnabled(false);
//		}
//	}
//}

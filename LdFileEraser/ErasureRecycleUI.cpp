#include "stdafx.h"
#include "ErasureRecycleUI.h"

/*
记录文件的隶属路径
用于统计目录下的文件被擦除进度，
*/
typedef struct RECYCLE_FILE_DATA
{
	PERASURE_FILE_DATA pFolder;
	CControlUI* ui;  
	DWORD nCount;
	DWORD nErasued;
}*PRECYCLE_FILE_DATA;

CErasureRecycleUI::CErasureRecycleUI():
	CShFileViewUI(),
	m_RecycleFiles(),
	m_EreaserThreads(this)
{
	btnOk = nullptr;
	m_Name = _T("ErasureRecycleUI");
	m_ItemSkin = _T("erasure\\listitem.xml");
}

CErasureRecycleUI::~CErasureRecycleUI()
{
	m_EreaserThreads.StopThreads();
	FreeRecycleFiles(&m_RecycleFiles);
}

void CErasureRecycleUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureRecycleUI::OnItemClick(TNotifyUI & msg)
{
	
}

DUI_BEGIN_MESSAGE_MAP(CErasureRecycleUI, CFramNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//回收站实际文件暂存
BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	if (_tcsicmp(pData->cFileName, _T("desktop.ini")) == 0)
		return true;

	CTreeNodes<PERASURE_FILE_DATA>* files = (CTreeNodes<PERASURE_FILE_DATA>*)Param;
	
	CLdString s = (LPTSTR)files->Tag;

	if (s[s.GetLength() - 1] != '\\')
		s += '\\';
	PERASURE_FILE_DATA p = new ERASURE_FILE_DATA;
	ZeroMemory(p, sizeof(ERASURE_FILE_DATA));
	s += pData->cFileName;
	s.CopyTo(p->cFileName);
	int index = files->Add(p);

	if ((pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		p->nFileType = eft_directory;

		CTreeNode<PERASURE_FILE_DATA>* node = files->Get(index);
		CTreeNodes<PERASURE_FILE_DATA>* cfs = node->GetItems();
		cfs->Tag = (UINT_PTR)s.GetData();

		CFileUtils::FindFile(s, _T("*.*"), this, (UINT_PTR)cfs);
		//cfs->Sort(this);
	}
	else
	{
		p->nFileType = eft_file;
	}
	return true;
}

//设置文件的目录指向
DWORD CErasureRecycleUI::SetFolderFilesData(PERASURE_FILE_DATA pFolder, CTreeNodes<ERASURE_FILE_DATA*>* files)
{
	DWORD result = files->GetCount();
	for(int i=0;i<files->GetCount();i++)
	{
		CTreeNode<PERASURE_FILE_DATA>* node = files->Get(i);
		PRECYCLE_FILE_DATA p = new RECYCLE_FILE_DATA;
		ZeroMemory(p, sizeof(RECYCLE_FILE_DATA));
		p->pFolder = pFolder;
		node->Get()->Tag = (UINT_PTR)p;
		if (node->GetCount() > 0)
		{
			result += SetFolderFilesData(node->Get(), node->GetItems());
		}
	}
	return result;
}

//找到虚拟文件并与实际文件对应，
BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);

	PERASURE_FILE_DATA* pinfo = m_RecycleFiles.Find(fi.szDisplayName, this);  //虚拟文件就在根目录
	if(pinfo)
	{
		PRECYCLE_FILE_DATA p = new RECYCLE_FILE_DATA;
		ZeroMemory(p, sizeof(RECYCLE_FILE_DATA));
		p->ui = AddRecord(pData);
		
		//设置文件的目录指向，便于擦除目录下的文件更新进度
		(*pinfo)->Tag = (UINT_PTR)p;
		CTreeNode<PERASURE_FILE_DATA>* node = m_RecycleFiles.Get(m_RecycleFiles.IndexOf(*pinfo));
		if(node->GetCount() > 0)
		{
			p->nCount = SetFolderFilesData(*pinfo, node->GetItems());			
		}
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

int CErasureRecycleUI::ArraySortCompare(ERASURE_FILE_DATA** it1, ERASURE_FILE_DATA** it2)
{
	int result;
	TCHAR file1[100] = { 0 };
	TCHAR file2[100] = { 0 };
	CFileUtils::ExtractFileName((*it1)->cFileName, file1);
	CFileUtils::ExtractFileName((*it2)->cFileName, file2);

//	result = (*it1)->nFileType - (*it2)->nFileType;
//	if (result != 0)
//		return result;
	result = _tcsicmp(file1, file2);
	return result;
}
int CErasureRecycleUI::ArrayFindCompare(PVOID key, ERASURE_FILE_DATA ** it)
{
	TCHAR file[100] = { 0 };
	CFileUtils::ExtractFileName((*it)->cFileName, file);
	return _tcsicmp((TCHAR*)key, file);
};

void CErasureRecycleUI::EnumRecyleFiels()
{
	DWORD oldMode;
	CLdArray<CLdString*> Volumes;
	CLdString sid;
	CLdString recyclePath;

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);
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
		m_RecycleFiles.Tag = (UINT_PTR)recyclePath.GetData();
		CFileUtils::FindFile(recyclePath, L"*.*", this, (UINT_PTR)&m_RecycleFiles);

		delete Volumes[i];
	}
	m_RecycleFiles.Sort(this);

	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::FreeRecycleFiles(CTreeNodes<PERASURE_FILE_DATA>* files)
{
	for(int i=0;i<files->GetCount();i++)
	{
		CTreeNode<PERASURE_FILE_DATA> * file = files->Get(i);
		if (file)
		{
			delete file->Get();
			if (file->GetCount())
				FreeRecycleFiles(file->GetItems());
		}
	}
}

void CErasureRecycleUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOk = (CButtonUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0);
	//回收站中的实际文件（c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//回收站的虚拟文件（原文件信息）
	AddFolder(CSIDL_BITBUCKET);
}

BOOL CErasureRecycleUI::GernalCallback_Callback(PERASE_CALLBACK_PARAM pData, UINT_PTR Param)
{
	PRECYCLE_FILE_DATA pFolderData = nullptr;// (PRECYCLE_FILE_DATA)pData->pData->Tag;
	CProgressUI* pui;

	switch(pData->op)
	{
	case eto_begin: 
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_start: 
		for (pFolderData = (PRECYCLE_FILE_DATA)pData->pData->Tag; pFolderData!=nullptr; pFolderData = (PRECYCLE_FILE_DATA)pFolderData->pFolder->Tag)
		{
			if (pFolderData->ui)
			{
				pui = (CProgressUI*)pFolderData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
				if (pui)
				{
					pui->SetVisible(true);
				}
			}
			if (pFolderData->pFolder == 0)
				break;
		}
		break;
	case eto_completed: 
		for (pFolderData = (PRECYCLE_FILE_DATA)pData->pData->Tag; pFolderData != nullptr; pFolderData = (PRECYCLE_FILE_DATA)pFolderData->pFolder->Tag)
		{
			pFolderData->nErasued++;
			if(pFolderData->pFolder)
				DebugOutput(L"%d %d %s\n", pFolderData->nErasued, pFolderData->nCount, pFolderData->pFolder->cFileName);
			if (pFolderData->ui)
			{
				pui = (CProgressUI*)pFolderData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
				if (pui)
				{
					if (pFolderData->nCount == 0 || pFolderData->nErasued > pFolderData->nCount)
						pui->SetValue(pui->GetMaxValue());
					else
						pui->SetValue(pFolderData->nErasued / pFolderData->nCount * 100);
				}
			}
			if (pFolderData->pFolder == 0)
				break;
		}
		break;
	case eto_progress: 
		pFolderData = (PRECYCLE_FILE_DATA)pData->pData->Tag;
		if (pFolderData && pFolderData->ui)
		{
			pui = (CProgressUI*)pFolderData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
			if (pui)
				pui->SetValue(pData->nPercent);
		}
		break;
	case eto_finished: 
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"OK");
		break;
	default: break;
	}
	return true;
}

void CErasureRecycleUI::StatErase()
{
	m_EreaserThreads.SetEreaureMethod(&CErasureMethod::Pseudorandom());
	m_EreaserThreads.SetEreaureFiles(&m_RecycleFiles);
	m_EreaserThreads.StartEreasure(10);
}

void CErasureRecycleUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag())
			StatErase();
		else
		{
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

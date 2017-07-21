#include "stdafx.h"
#include "ErasureRecycleUI.h"
#include <thread>

CErasureRecycleUI::CErasureRecycleUI():
	CShFileViewUI(),
	m_ErasureFiles(500),
	m_EreaserThreads(nullptr)
{
	btnOk = nullptr;
	m_ItemSkin = _T("erasure\\listitem.xml");
}


CErasureRecycleUI::~CErasureRecycleUI()
{
	for(int i=0; i<m_ErasureFiles.GetCount(); i++)
	{
		PERASURE_FILE_DATA p;
		if(m_ErasureFiles.GetValueAt(i, p))
			delete p;
	}

	m_EreaserThreads.StopThreads();
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

	CLdString s = (LPTSTR)Param;
	PERASURE_FILE_DATA p = new ERASURE_FILE_DATA;
	ZeroMemory(p, sizeof(ERASURE_FILE_DATA));
	s += pData->cFileName;
	s.CopyTo(p->cFileName);
	
	if ((pData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		p->nFileType = eft_directory;
		CFileUtils::FindFile(s, _T("*.*"), this, (UINT_PTR)s.GetData());
	}
	else
		p->nFileType = eft_file;
	
	m_ErasureFiles.Set(pData->cFileName, p);
	return true;
}

//找到虚拟文件并与实际文件对应，
BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	//__super::GernalCallback_Callback(pData, Param);

	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);
//	if (pData->GetCount() > lstFile->GetHeader()->GetCount() + 1)
//		AddLstViewHeader(pData->GetCount() + 1);

	PERASURE_FILE_DATA pinfo = nullptr;
	if(m_ErasureFiles.Find(fi.szDisplayName, pinfo))
	{
		AddRecord(pData);
	}
	return true;
}

//枚举盘符暂存用于查找每个盘下的回收站文件
BOOL CErasureRecycleUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CLdArray<CLdString*>* pVolumes = (CLdArray<CLdString*>*)Param;
	pVolumes->Add(new CLdString(pData));
	return TRUE;
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

		CFileUtils::FindFile(recyclePath, L"*", this, (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}

	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::OnInit()
{
	__super::OnInit();

//	btnOk = static_cast<CButtonUI*>(GetUI()->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));
	//回收站中的实际文件（c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//回收站的虚拟文件（原文件信息）
	//AddFolder(CSIDL_BITBUCKET);
}

BOOL CErasureRecycleUI::GernalCallback_Callback(PERASE_CALLBACK_PARAM pData, UINT_PTR Param)
{
	switch(pData->op)
	{
	case eto_begin: 
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_start: 
		break;
	case eto_completed: 
//		CListContainerElementUI* ui = reinterpret_cast<CListContainerElementUI*>(Param);
//		lstFile->Remove(ui);
		break;
	case eto_progress: 
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

void CErasureRecycleUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag() )
			m_EreaserThreads.StartEreasure(10);
		else
		{
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

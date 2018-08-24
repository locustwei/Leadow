#include "stdafx.h"
#include "ErasureRecycleUI.h"

CErasureRecycleUI::CErasureRecycleUI():
	CErasureFileUI()
{
	m_Name = _T("ErasureRecycleUI");
	m_ItemSkin = _T("erasure/listitem_recycle.xml");            //
}

CErasureRecycleUI::~CErasureRecycleUI()
{
}

DUI_BEGIN_MESSAGE_MAP(CErasureRecycleUI, CErasureFileUI)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
//DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

//����վʵ���ļ�
BOOL CErasureRecycleUI::EnumRecycleFile_Callback(PVOID data, UINT_PTR Param)
{
	LPWIN32_FIND_DATA pData = (LPWIN32_FIND_DATA)data;

	if (_tcsicmp(pData->cFileName, _T("desktop.ini")) == 0)
		return true;

	CLdString FileName = (TCHAR*)Param;
	FileName += pData->cFileName;

	AddEraseFile(FileName);

	return true;
}

//�ҵ������ļ�����ʵ���ļ���Ӧ��
BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);

	CVirtualFile* file = m_ErasureFile.Find(fi.szDisplayName);

	if (file)
	{
		//AddFileUI(file);
	}

	return true;
}

//ö���̷��ݴ����ڲ���ÿ�����µĻ���վ�ļ�
BOOL CErasureRecycleUI::EnumVolume_Callback(PVOID data, UINT_PTR Param)
{
	TCHAR* pData = (TCHAR*)data;

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

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);  //��ֹ�����ļ�����Ի���
	WIN_OS_TYPE os = GetOsType();
	if (GetCurrentUserSID(sid) != 0)
		return;
	CVolumeUtils::MountedVolumes(CMethodDelegate::MakeDelegate(this, &CErasureRecycleUI::EnumVolume_Callback), (UINT_PTR)&Volumes);

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
		CFileUtils::EnumFiles(recyclePath, L"*.*", CMethodDelegate::MakeDelegate(this, &CErasureRecycleUI::EnumRecycleFile_Callback), (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}
	m_ErasureFile.Sort();

	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::AttanchControl(CControlUI* pCtrl)
{
	CShFileViewUI::AttanchControl(pCtrl); //����__super::��ΪCErasureFileUIҲ���Ĭ����
	btnOk = static_cast<CButtonUI*>(m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));

	//����վ�е�ʵ���ļ���c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//����վ�������ļ���ԭ�ļ���Ϣ��
	AddFolder(CSIDL_BITBUCKET);
	if (lstFile->GetCount() == 0)
		AddLstViewHeader(5);
}
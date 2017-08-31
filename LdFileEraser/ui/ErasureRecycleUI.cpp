#include "stdafx.h"
#include "ErasureRecycleUI.h"

CErasureRecycleUI::CErasureRecycleUI():
	CErasureFileUI()
{
	m_Name = _T("ErasureRecycleUI");
	m_ItemSkin = _T("erasure\\listitem_recycle.xml");            //
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
BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
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
		AddFileUI(file, pData);
	}

	return true;
}

//ö���̷��ݴ����ڲ���ÿ�����µĻ���վ�ļ�
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

	SetThreadErrorMode(SEM_FAILCRITICALERRORS, &oldMode);  //��ֹ�����ļ�����Ի���
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

void CErasureRecycleUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	//����վ�е�ʵ���ļ���c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//����վ�������ļ���ԭ�ļ���Ϣ��
	AddFolder(CSIDL_BITBUCKET);
	if (lstFile->GetCount() == 0)
		AddLstViewHeader(5);
}
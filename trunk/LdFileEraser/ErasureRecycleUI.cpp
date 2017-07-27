#include "stdafx.h"
#include "ErasureRecycleUI.h"

/*
��¼�ļ�������·��
����ͳ��Ŀ¼�µ��ļ����������ȣ�
*/

CErasureRecycleUI::CErasureRecycleUI():
	CShFileViewUI(),
	m_RecycleFile(),
	m_EreaserThreads(this)
{
	btnOk = nullptr;
	m_Name = _T("ErasureRecycleUI");
	m_ItemSkin = _T("erasure\\listitem.xml");            //
}

CErasureRecycleUI::~CErasureRecycleUI()
{
	m_EreaserThreads.StopThreads();
	FreeRecycleFiles(m_RecycleFile.GetFiles());
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

//����վʵ���ļ�
BOOL CErasureRecycleUI::GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param)
{
	if (_tcsicmp(pData->cFileName, _T("desktop.ini")) == 0)
		return true;
	CFileInfo* file = new CFileInfo();
	file->SetFindData((TCHAR*)Param, pData);
	
	if (file->IsDirectory())
	{
		file->FindFiles(true);
	}
	m_RecycleFile.GetFiles()->Add(file);

	return true;
}

//�����ļ���Ŀ¼ָ�򣬲���ʱ���������ļ��еĽ���
DWORD CErasureRecycleUI::SetFolderFilesData(CLdArray<CFileInfo*>* files)
{
	DWORD result = files->GetCount();
	for (int i = 0; i < files->GetCount(); i++)
	{
		CFileInfo* file = files->Get(i);

		PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
		ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
		file->SetTag((UINT_PTR)p);
		if (file->IsDirectory())
		{
			DWORD count = SetFolderFilesData(file->GetFiles());
			p->nCount = count;
			result += count;
		}
	}

	return result;
}

//�ҵ������ļ�����ʵ���ļ���Ӧ��
BOOL CErasureRecycleUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	SHGetFileInfo(pData->Get(0), 0, &fi, sizeof(fi), SHGFI_DISPLAYNAME | SHGFI_PIDL);

	CFileInfo* file = m_RecycleFile.Find(fi.szDisplayName);
	if (file)
	{
		PFILE_ERASURE_DATA p = new FILE_ERASURE_DATA;
		ZeroMemory(p, sizeof(FILE_ERASURE_DATA));
		p->ui = AddRecord(pData);
		file->SetTag((UINT_PTR)p);
		if (file->IsDirectory())
		{
			p->nCount = SetFolderFilesData(file->GetFiles());
		}
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
//		m_RecycleFiles.Tag = (UINT_PTR)recyclePath.GetData();
		CFileUtils::FindFile(recyclePath, L"*.*", this, (UINT_PTR)recyclePath.GetData());

		delete Volumes[i];
	}
	m_RecycleFile.Sort();

	SetThreadErrorMode(oldMode, &oldMode);
}

void CErasureRecycleUI::FreeRecycleFiles(CLdArray<CFileInfo*>* files)
{
	for(int i=0;i<files->GetCount();i++)
	{
		CFileInfo * file = files->Get(i);
		if (file)
		{
			PFILE_ERASURE_DATA pData = (PFILE_ERASURE_DATA)file->GetTag();
			if(pData)
			{
				delete pData;
			}
			file->SetTag(0);
			if (file->IsDirectory())
				FreeRecycleFiles(file->GetFiles());
		}
	}
}

void CErasureRecycleUI::AttanchControl(CControlUI* pCtrl)
{
	__super::AttanchControl(pCtrl);
	btnOk = (CButtonUI*)m_Ctrl->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0);
	//����վ�е�ʵ���ļ���c:\$RECYCLED.BIN\sid\*)
	EnumRecyleFiels();
	//����վ�������ļ���ԭ�ļ���Ϣ��
	AddFolder(CSIDL_BITBUCKET);
}

bool CErasureRecycleUI::EraserThreadCallback(CFileInfo* pFile, E_THREAD_OPTION op, DWORD dwValue)
{
	PFILE_ERASURE_DATA pEraserData;
	CProgressUI* pui;

	switch(op)
	{
	case eto_start:  //�ܽ��ȿ�ʼ
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
		break;
	case eto_begin:  //�����ļ���ʼ
		for (CFileInfo* p = pFile; p!=nullptr; p=p->GetFolder())
		{//�ҵ������ļ��ж�Ӧlistview�У���ʾ������
			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());

			if (pEraserData && pEraserData->ui)
			{
				pui = (CProgressUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
				if (pui)
				{
					pui->SetVisible(true);
				}
			}
		}
		break;
	case eto_completed: //�����ļ��������
		//���ò���״̬
		pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
		if (dwValue == 0)
		{
			if (pEraserData)
			{
				pEraserData->nStatus = efs_erasured;
			}
		}
		else
		{
			pEraserData = (PFILE_ERASURE_DATA)(pFile->GetTag());
			if (pEraserData)
			{
				pEraserData->nStatus = efs_error;
				pEraserData->nErrorCode = dwValue;
			}
		}
		for (CFileInfo* p = pFile; p != nullptr; p = p->GetFolder())
		{
			pEraserData = (PFILE_ERASURE_DATA)(p->GetTag());
			if (pEraserData)
				InterlockedIncrement(&pEraserData->nErasued);
			//���������ļ��н���
			if (pEraserData && pEraserData->ui)
			{
				pui = (CProgressUI*)pEraserData->ui->FindControl(CDuiUtils::FindControlByNameProc, _T("progress"), 0);
				if (pui)
				{
					if (pEraserData->nCount == 0 || pEraserData->nErasued > pEraserData->nCount)
						pui->SetValue(pui->GetMaxValue());
					else
						pui->SetValue(pEraserData->nErasued * 100 / pEraserData->nCount);
				}
			}
		}
		break;
	case eto_progress: //�����ļ�����
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
}

void CErasureRecycleUI::StatErase()
{
	m_EreaserThreads.SetEreaureMethod(&CErasureMethod::Pseudorandom());
	m_EreaserThreads.SetEreaureFiles(m_RecycleFile.GetFiles());
	m_EreaserThreads.StartEreasure(20);
}

void CErasureRecycleUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
		if (!btnOk->GetTag())
			StatErase();
		else
		{//����û�н�����ȡ����ǰ����
			m_EreaserThreads.StopThreads();
			btnOk->SetEnabled(false);
		}
	}
}

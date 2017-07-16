#include "stdafx.h"
#include "ErasureVolumeUI.h"
#include <thread>

CErasureVolumeUI::CErasureVolumeUI():
	m_Columes(),
	m_ErasureFiles(500),
	m_EreaserThreads()
{
	btnOk = nullptr;
	lstVolume = nullptr;
	BuildXml(_T("erasure\\erasurevolume.xml"), nullptr);
}


CErasureVolumeUI::~CErasureVolumeUI()
{
	for(int i=0; i<m_Columes.GetCount();i++)
	{
		delete m_Columes[i]->szName;
		delete m_Columes[i];
	}

	for(int i=0; i<m_ErasureFiles.GetCount(); i++)
	{
		PERASURE_FILE_DATA p;
		if(m_ErasureFiles.GetValueAt(i, p))
			delete p;
	}

	m_EreaserThreads.StopThreads();
	while (m_EreaserThreads.GetCount() > 0)
		Sleep(10); //�ȴ������߳̽���

}
//���������ļ������߳�
void CErasureVolumeUI::ErasureAllFiles(CThread* Sender)
{
	for (int i = 0; i < m_ErasureFiles.GetCount(); i++)
	{//�Ȳ����ļ�
		if (Sender->GetTerminated())
			break;

		PERASURE_FILE_DATA pinfo;
		CLdString s = m_ErasureFiles.GetAt(i);

		if(!m_ErasureFiles.GetValueAt(i, pinfo) || pinfo->Ereased || pinfo->IsDirectory)
			continue;
		
		while (m_EreaserThreads.GetCount() >= MAX_THREAD_COUNT)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}

		m_EreaserThreads.AddThread(this, UINT_PTR(m_ErasureFiles.GetAt(i)));
	}
	if (Sender->GetTerminated())
		return;

	for (int i = 0; i < m_ErasureFiles.GetCount(); i++)
	{//Ŀ¼
		if (Sender->GetTerminated())
			break;

		PERASURE_FILE_DATA pinfo;
		CLdString s = m_ErasureFiles.GetAt(i);

		if (!m_ErasureFiles.GetValueAt(i, pinfo) || pinfo->Ereased || !pinfo->IsDirectory)
			continue;

		while (m_EreaserThreads.GetCount() >= MAX_THREAD_COUNT)
		{
			Sleep(50);
			if (Sender->GetTerminated())
				break;
		}

		m_EreaserThreads.AddThread(this, UINT_PTR(m_ErasureFiles.GetAt(i)));
	}
}

void CErasureVolumeUI::ErasureSingleFile(CThread* Sender, TCHAR* Key)
{
	if (Sender->GetTerminated())
		return;

	PERASURE_FILE_DATA pinfo;
	m_ErasureFiles.Find(Key, pinfo);
	CErasureThread* pProcess = (CErasureThread*)Sender;

	CListContainerElementUI* ui = (CListContainerElementUI*)pinfo->ListRow;
	if (ui)
	{//��ʾ������
		pProcess->ui = (CProgressUI*)ui->FindSubControl(_T("progress"));
		if (pProcess->ui) 
			pProcess->ui->SetVisible(true);
	}
	CErasure erasure;
	DWORD r = erasure.FileErasure(pinfo->cFileName, CErasureMethod::Pseudorandom(), pProcess);
	
	if(r == 0)
	{
		pinfo->Ereased = true;
		if (ui)
			CLdApp::Send2MainThread(this, (UINT_PTR)ui);
	}

}

VOID CErasureVolumeUI::ThreadRun(CThread* Sender, UINT_PTR Param)
{
	if (Param == 0)
		ErasureAllFiles(Sender);  //�����ļ������߳�
	else
		ErasureSingleFile(Sender, (TCHAR*)Param); //�����ļ������߳�
}

VOID CErasureVolumeUI::OnThreadInit(CThread* Sender, UINT_PTR Param)
{
	if (Param == 0) 
	{ //���Ǵ��������ļ��̵߳��߳�
		btnOk->SetTag(1);
		btnOk->SetText(L"Cancel");
	}
}
//�����߳̽���
VOID CErasureVolumeUI::OnThreadTerminated(CThread* Sender, UINT_PTR Param)
{
	m_EreaserThreads.RemoveThread(Sender);
	if (m_EreaserThreads.GetCount() == 0)
	{ //�����̶߳�����
		btnOk->SetTag(0);
		btnOk->SetEnabled(true);
		btnOk->SetText(L"OK");
	}
}

void CErasureVolumeUI::OnSelectChanged(TNotifyUI & msg)
{
}

void CErasureVolumeUI::OnItemClick(TNotifyUI & msg)
{
	
}

DUI_BEGIN_MESSAGE_MAP(CErasureVolumeUI, CFramWnd)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()
//���߳���ɾ���Ѳ������ļ���ӦlstFile��
BOOL CErasureVolumeUI::GernalCallback_Callback(LPVOID pData, UINT_PTR Param)
{
	CListContainerElementUI* ui = reinterpret_cast<CListContainerElementUI*>(Param);
	//lstVolume->Remove(ui);
	return true;
}

//lstFile�����ͷ
void CErasureVolumeUI::AddLstViewHeader(int ncount)
{
	CControlUI* col = lstVolume->GetHeader()->GetItemAt(0);
	col->SetVisible(true);
	while(lstVolume->GetHeader()->GetCount()<ncount)
	{
		CControlUI* col1 = col->CloneNew();
		lstVolume->GetHeader()->Add(col1);
	}
	for(int i=0; i<ncount; i++)
	{
		if (i >= m_Columes.GetCount())
			break;
		col = lstVolume->GetHeader()->GetItemAt(i);
		col->SetText(m_Columes[i]->szName);
		col->SetFixedWidth(m_Columes[i]->cxChar * 10);
	}
}
//�ҵ������ļ�����ʵ���ļ���Ӧ��
BOOL CErasureVolumeUI::GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param)
{
	SHFILEINFO fi;
	if (pData->GetCount() == 0 || pData->Get(0) == nullptr)
		return true;
	
	if (pData->GetCount() > lstVolume->GetHeader()->GetCount() + 1)
		AddLstViewHeader(pData->GetCount() + 1);

	PERASURE_FILE_DATA pinfo = nullptr;

	CListContainerElementUI* item = static_cast<CListContainerElementUI*>(lstVolume->GetItemAt(0));
	if (item->GetTag() != 0)
	{
		item = static_cast<CListContainerElementUI*>(item->CloneNew());
		lstVolume->Add(item);
	}
	item->SetVisible(true);
	item->SetTag((UINT_PTR)123);
	if(pinfo)
		pinfo->ListRow = item;
	for (int i = 1; i<pData->GetCount(); i++)
	{
		CControlUI* ui = item->GetItemAt(i-1);
		if (ui)
		{
			CControlUI* cap = ui->FindControl(CDuiUtils::FindControlByNameProc, _T("caption"), 0);
			if (cap)
				cap->SetText(pData->Get(i));
			else
				ui->SetText(pData->Get(i));

		}
	}

	return true;
}
//��ȡ����Ϣ�ݴ�
BOOL CErasureVolumeUI::GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param)
{
	PSH_HEAD_INFO p = new SH_HEAD_INFO;
	p->cxChar = pData->cxChar;
	p->fmt = pData->fmt;
	p->szName = new TCHAR[_tcslen(pData->szName)+1];
	_tcscpy(p->szName, pData->szName);
	m_Columes.Add(p);
	return true;
};
//ö���̷��ݴ����ڲ���ÿ�����µĻ���վ�ļ�
BOOL CErasureVolumeUI::GernalCallback_Callback(TCHAR* pData, UINT_PTR Param)
{
	CLdArray<CLdString*>* pVolumes = (CLdArray<CLdString*>*)Param;
	pVolumes->Add(new CLdString(pData));
	return TRUE;
};

void CErasureVolumeUI::OnInit()
{
	btnOk = static_cast<CButtonUI*>(GetUI()->FindControl(CDuiUtils::FindControlByNameProc, _T("btnOk"), 0));
	lstVolume = static_cast<CListUI*>(GetUI()->FindControl(CDuiUtils::FindControlByNameProc, _T("listview"), 0));
	CSHFolders::EnumFolderColumes(CSIDL_DRIVES, this, 0);

	CSHFolders::EnumFolderObjects(CSIDL_DRIVES, this, 0);
}

void CErasureVolumeUI::OnClick(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.pSender == btnOk)
	{
//		if (!btnOk->GetTag() && m_EreaserThreads.GetCount() == 0)
//			m_EreaserThreads.AddThread(this, 0);
//		else
//		{
//			m_EreaserThreads.StopThreads();
//			btnOk->SetEnabled(false);
//		}
	}
}

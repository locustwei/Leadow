#include "stdafx.h"
#include "ErasureImpl.h"
#include "eraser/ErasureThread.h"


CErasureImpl* ThisLibrary = nullptr;

CErasureImpl::CErasureImpl()
	: m_MainWnd(nullptr)
	, m_Ctrl(nullptr)
	, m_Config(nullptr)
	, m_hModule(nullptr)
{
}

CErasureImpl::~CErasureImpl()
{
	if (m_MainWnd)
	{
		delete m_MainWnd;
		m_MainWnd = nullptr;
	}
	if(m_Config)
	{
		m_Config->SaveConfig();
		delete m_Config;
	}

	ThisLibrary = nullptr;
}

CErasureConfig* CErasureImpl::GetConfig()
{
	if(!m_Config)
	{
		m_Config = new CErasureConfig();
		m_Config->LoadConfig();
	}
	return m_Config;
}

HMODULE CErasureImpl::GetModuleHandleW()
{
	return m_hModule;
}

CFramNotifyPump* CErasureImpl::GetNotifyPump()
{
	if (!m_MainWnd)
	{
		m_MainWnd = new CErasureMainWnd();
	}
	assert(m_MainWnd);

	return m_MainWnd;
}

TCHAR* CErasureImpl::UIResorce()
{
	return _T("erasure/erasuremain.xml");
}

void CErasureImpl::SetUI(CControlUI* pCtrl)
{
	m_Ctrl = pCtrl;
	GetNotifyPump()->AttanchControl(m_Ctrl);
}

CControlUI* CErasureImpl::GetUI()
{
	return m_Ctrl;
}

DWORD CErasureImpl::EraseFile(CLdConfig Param, IEraserThreadCallback* callback)
{
	CFolderInfo m_ErasureFile;

	int mothed;
	if (Param.GetDataType(EPN_MOTHED) == JsonBox::Value::NULL_VALUE)
		mothed = ThisLibrary->GetConfig()->GetFileErasureMothed();
	else
		mothed = Param.GetInteger(EPN_MOTHED, 3);
	BOOL removefolder;
	if (Param.GetDataType(EPN_UNDELFOLDER) == JsonBox::Value::NULL_VALUE)
		removefolder = ThisLibrary->GetConfig()->IsRemoveFolder();
	else
		removefolder = Param.GetBoolean(EPN_UNDELFOLDER, true);
	int k = Param.GetArrayCount(EPN_FILE);
	for(int i=0; i<k; i++)
	{
		CLdString s = Param.GetString(EPN_FILE, nullptr, i);
		if (s.IsEmpty())
			continue;
		CFileInfo* info;
		if (CFileUtils::IsDirectoryExists(s))
		{
			info = new CFolderInfo();
			info->SetFileName(s);
			((CFolderInfo*)info)->FindFiles(true);
		}
		else
		{
			info = new CFileInfo();
			info->SetFileName(s);
		}

		m_ErasureFile.AddFile(info);
	}
	CEreaserThrads m_EreaserThreads(callback);
	m_EreaserThreads.SetEreaureFiles(m_ErasureFile.GetFiles());
	m_EreaserThreads.StartEreasure(10);

	return 0;
}

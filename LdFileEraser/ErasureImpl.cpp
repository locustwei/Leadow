#include "stdafx.h"
#include "ErasureImpl.h"
#include "eraser/ErasureThread.h"


CErasureImpl* ThisLibrary = nullptr;

CErasureImpl::CErasureImpl()
	//: m_MainWnd(nullptr)
	//, m_Ctrl(nullptr)
	//, m_Config(nullptr)
	: m_hModule(nullptr)
	, m_EraseThread()
	, m_Files()
{
}

CErasureImpl::~CErasureImpl()
{
//	if (m_MainWnd)
//	{
//		delete m_MainWnd;
//		m_MainWnd = nullptr;
//	}
//	if(m_Config)
//	{
//		m_Config->SaveConfig();
//		delete m_Config;
//	}

	ThisLibrary = nullptr;
}

//CErasureConfig* CErasureImpl::GetConfig()
//{
//	if(!m_Config)
//	{
//		m_Config = new CErasureConfig();
//		m_Config->LoadConfig();
//	}
//	return m_Config;
//}

HMODULE CErasureImpl::GetModuleHandleW()
{
	return m_hModule;
}

//CFramNotifyPump* CErasureImpl::GetNotifyPump()
//{
//	if (!m_MainWnd)
//	{
//		m_MainWnd = new CErasureMainWnd();
//	}
//	assert(m_MainWnd);
//
//	return m_MainWnd;
//}
//
//TCHAR* CErasureImpl::UIResorce()
//{
//	return _T("erasure/erasuremain.xml");
//}
//
//void CErasureImpl::SetUI(CControlUI* pCtrl)
//{
//	m_Ctrl = pCtrl;
//	GetNotifyPump()->AttanchControl(m_Ctrl);
//}
//
//CControlUI* CErasureImpl::GetUI()
//{
//	return m_Ctrl;
//}

DWORD CErasureImpl::EraseFile(CLdConfig& Param, IEraserThreadCallback* callback)
{
	int mothed = Param.GetInteger(EPN_MOTHED, 3);
	BOOL removefolder = Param.GetBoolean(EPN_UNDELFOLDER, true);
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

		m_Files.Add(info);
	}

	m_EraseThread.SetCallback(callback);
	m_EraseThread.SetEreaureFiles(&m_Files);
	m_EraseThread.StartEreasure(10);

	return 0;
}

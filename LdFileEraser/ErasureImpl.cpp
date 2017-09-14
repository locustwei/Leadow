#include "stdafx.h"
#include "ErasureImpl.h"


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

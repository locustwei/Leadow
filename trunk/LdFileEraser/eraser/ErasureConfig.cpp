#include "stdafx.h"
#include "ErasureConfig.h"

CErasureConfig::CErasureConfig():CLdConfig()
{
	CLdString& appPath = CLdApp::ThisApp->GetAppDataPath();
	m_ConfigFileName = appPath;
	m_ConfigFileName += "rderasure.cng";
}


CErasureConfig::~CErasureConfig()
{
}

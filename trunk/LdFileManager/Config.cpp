#include "stdafx.h"
#include "Config.h"

CConfig* AppConfig = nullptr;


CConfig::CConfig():CLdConfig()
{
	CLdString& appPath = ThisApp->GetAppDataPath();
	m_ConfigFileName = appPath;
	m_ConfigFileName += "dydot.cng";
}


CConfig::~CConfig()
{
}

int CConfig::GetFileErasureMothed()
{
	return GetInteger("erasure/file/mothed", 3);
}

BOOL CConfig::IsRemoveFolder()
{
	return GetBoolean("erasure/file/removefolder", true);
}

int CConfig::GetVolumeErasureMethed()
{
	return GetInteger("erasure/volume/mothed", 0);
}

BOOL CConfig::IsSkipSpace()
{
	return GetBoolean("erasure/volume/skipspace", false);;
}

BOOL CConfig::IsSkipTrack()
{
	return GetBoolean("erasure/volume/skiptrack", false);;
}

BOOL CConfig::IsErasureFreeFileSpace()
{
	return GetBoolean("erasure/volume/filefree", false);;
}

BOOL CConfig::IsShutDown()
{
	return GetBoolean("erasure/volume/shutdown", false);;
}

void CConfig::SetFileErasureMothed(int Mothed)
{
	AddConfigObject("erasure/file/mothed", Mothed);
}

void CConfig::SetRemoveFolder(BOOL value)
{
	AddConfigObject("erasure/file/removefolder", value);
}

void CConfig::SetVolumeErasureMethed(int Mothed)
{
	AddConfigObject("erasure/volume/mothed", Mothed);
}

void CConfig::SetSkipSpace(BOOL value)
{
	AddConfigObject("erasure/volume/skipspace", value);
}

void CConfig::SetSkipTrack(BOOL value)
{
	AddConfigObject("erasure/volume/skiptrack", value);
}

void CConfig::SetErasureFreeFileSpace(BOOL value)
{
	AddConfigObject("erasure/volume/filefree", value);
}

void CConfig::SetShutDown(BOOL value)
{
	AddConfigObject("erasure/volume/shutdown", value);
}

void CConfig::LoadConfig()
{
	if (AppConfig)
		return;
	AppConfig = new CConfig;
	AppConfig->LoadConfig();
}

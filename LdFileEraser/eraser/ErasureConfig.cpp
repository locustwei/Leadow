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

ErasureMothedType CErasureConfig::GetFileErasureMothed()
{
	return (ErasureMothedType)GetInteger("file/mothed", 3);
}

BOOL CErasureConfig::IsRemoveFolder()
{
	return GetBoolean("file/removefolder", true);
}

ErasureMothedType CErasureConfig::GetVolumeErasureMethed()
{
	return (ErasureMothedType)GetInteger("volume/mothed", 0);
}

BOOL CErasureConfig::IsSkipSpace()
{
	return GetBoolean("volume/skipspace", false);;
}

BOOL CErasureConfig::IsSkipTrack()
{
	return GetBoolean("volume/skiptrack", false);;
}

BOOL CErasureConfig::IsErasureFreeFileSpace()
{
	return GetBoolean("volume/filefree", false);;
}

BOOL CErasureConfig::IsShutDown()
{
	return GetBoolean("volume/shutdown", false);;
}

void CErasureConfig::SetFileErasureMothed(ErasureMothedType Mothed)
{
	AddConfigObject("file/mothed", Mothed);
}

void CErasureConfig::SetRemoveFolder(BOOL value)
{
	AddConfigObject("file/removefolder", value);
}

void CErasureConfig::SetVolumeErasureMethed(ErasureMothedType Mothed)
{
	AddConfigObject("volume/mothed", Mothed);
}

void CErasureConfig::SetSkipSpace(BOOL value)
{
	AddConfigObject("volume/skipspace", value);
}

void CErasureConfig::SetSkipTrack(BOOL value)
{
	AddConfigObject("volume/skiptrack", value);
}

void CErasureConfig::SetErasureFreeFileSpace(BOOL value)
{
	AddConfigObject("volume/filefree", value);
}

void CErasureConfig::SetShutDown(BOOL value)
{
	AddConfigObject("volume/shutdown", value);
}

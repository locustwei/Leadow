#include "stdafx.h"
#include "Config.h"

CAppConfig::CAppConfig():CDynObject()
{
}


CAppConfig::~CAppConfig()
{
}

CErasureConfig::CErasureConfig():CDynObject()
{
}

CErasureConfig::~CErasureConfig()
{
}

int CErasureConfig::GetFileErasureMothed()
{
	return GetInteger("erasure/file/mothed", 3);
}

BOOL CErasureConfig::IsRemoveFolder()
{
	return GetBoolean("erasure/file/removefolder", true);
}

int CErasureConfig::GetVolumeErasureMethed()
{
	return GetInteger("erasure/volume/mothed", 0);
}

BOOL CErasureConfig::IsSkipSpace()
{
	return GetBoolean("erasure/volume/skipspace", false);;
}

BOOL CErasureConfig::IsSkipTrack()
{
	return GetBoolean("erasure/volume/skiptrack", false);;
}

BOOL CErasureConfig::IsErasureFreeFileSpace()
{
	return GetBoolean("erasure/volume/filefree", false);;
}

BOOL CErasureConfig::IsShutDown()
{
	return GetBoolean("erasure/volume/shutdown", false);;
}

void CErasureConfig::SetFileErasureMothed(int Mothed)
{
	AddObjectAttribute("erasure/file/mothed", Mothed);
}

void CErasureConfig::SetRemoveFolder(BOOL value)
{
	AddObjectAttribute("erasure/file/removefolder", value);
}

void CErasureConfig::SetVolumeErasureMethed(int Mothed)
{
	AddObjectAttribute("erasure/volume/mothed", Mothed);
}

void CErasureConfig::SetSkipSpace(BOOL value)
{
	AddObjectAttribute("erasure/volume/skipspace", value);
}

void CErasureConfig::SetSkipTrack(BOOL value)
{
	AddObjectAttribute("erasure/volume/skiptrack", value);
}

void CErasureConfig::SetErasureFreeFileSpace(BOOL value)
{
	AddObjectAttribute("erasure/volume/filefree", value);
}

void CErasureConfig::SetShutDown(BOOL value)
{
	AddObjectAttribute("erasure/volume/shutdown", value);
}

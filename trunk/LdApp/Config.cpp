#include "stdafx.h"
#include "Config.h"

CConfig::CConfig():CDynObject()
{
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
	AddObjectAttribute("erasure/file/mothed", Mothed);
}

void CConfig::SetRemoveFolder(BOOL value)
{
	AddObjectAttribute("erasure/file/removefolder", value);
}

void CConfig::SetVolumeErasureMethed(int Mothed)
{
	AddObjectAttribute("erasure/volume/mothed", Mothed);
}

void CConfig::SetSkipSpace(BOOL value)
{
	AddObjectAttribute("erasure/volume/skipspace", value);
}

void CConfig::SetSkipTrack(BOOL value)
{
	AddObjectAttribute("erasure/volume/skiptrack", value);
}

void CConfig::SetErasureFreeFileSpace(BOOL value)
{
	AddObjectAttribute("erasure/volume/filefree", value);
}

void CConfig::SetShutDown(BOOL value)
{
	AddObjectAttribute("erasure/volume/shutdown", value);
}
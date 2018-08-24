#include "stdafx.h"
#include "ErasureConfig.h"

CErasureConfig ErasureConfig;

CErasureConfig::CErasureConfig():CDynObject()
{
}

CErasureConfig::~CErasureConfig()
{
}

int CErasureConfig::GetFileErasureMothed()
{
	return GetInteger(_T("erasure/file/mothed"), 3);
}

BOOL CErasureConfig::IsRemoveFolder()
{
	return GetBoolean(_T("erasure/file/removefolder"), true);
}

int CErasureConfig::GetVolumeErasureMethed()
{
	return GetInteger(_T("erasure/volume/mothed"), 0);
}

BOOL CErasureConfig::IsSkipSpace()
{
	return GetBoolean(_T("erasure/volume/skipspace"), false);;
}

BOOL CErasureConfig::IsSkipTrack()
{
	return GetBoolean(_T("erasure/volume/skiptrack"), false);;
}

BOOL CErasureConfig::IsErasureFreeFileSpace()
{
	return GetBoolean(_T("erasure/volume/filefree"), false);;
}

BOOL CErasureConfig::IsShutDown()
{
	return GetBoolean(_T("erasure/volume/shutdown"), false);;
}

void CErasureConfig::SetFileErasureMothed(int Mothed)
{
	AddObjectAttribute(_T("erasure/file/mothed"), Mothed);
}

void CErasureConfig::SetRemoveFolder(BOOL value)
{
	AddObjectAttribute(_T("erasure/file/removefolder"), value);
}

void CErasureConfig::SetVolumeErasureMethed(int Mothed)
{
	AddObjectAttribute(_T("erasure/volume/mothed"), Mothed);
}

void CErasureConfig::SetSkipSpace(BOOL value)
{
	AddObjectAttribute(_T("erasure/volume/skipspace"), value);
}

void CErasureConfig::SetSkipTrack(BOOL value)
{
	AddObjectAttribute(_T("erasure/volume/skiptrack"), value);
}

void CErasureConfig::SetErasureFreeFileSpace(BOOL value)
{
	AddObjectAttribute(_T("erasure/volume/filefree"), value);
}

void CErasureConfig::SetShutDown(BOOL value)
{
	AddObjectAttribute(_T("erasure/volume/shutdown"), value);
}

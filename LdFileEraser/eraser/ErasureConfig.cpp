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

CErasureMethod& CErasureConfig::GetFileErasureMothed()
{
	int k = GetFileErasureMothedIndex();
	switch(k)
	{
	case 0: return CErasureMethod::Pseudorandom();      
	case 1: return CErasureMethod::HMGIS5Baseline();    
	case 2: return CErasureMethod::GOSTP50739();        
	case 3: return CErasureMethod::DoD_E();             
	case 4: return CErasureMethod::USAF5020();          
	case 5: return CErasureMethod::USArmyAR380_19();    
	case 6: return CErasureMethod::HMGIS5Enhanced();    
	case 7: return CErasureMethod::DoD_EcE();           
	case 8: return CErasureMethod::RCMP_TSSIT_OPS_II(); 
	case 9: return CErasureMethod::Schneier();          
	case 10: return CErasureMethod::VSITR();            
	case 11: return CErasureMethod::Gutmann();          
	default:
		return CErasureMethod::Pseudorandom();          
	}
}

int CErasureConfig::GetFileErasureMothedIndex()
{
	return GetInteger("file/mothed", 3);
}

BOOL CErasureConfig::IsRemoveFolder()
{
	return GetBoolean("file/removefolder", true);
}

CErasureMethod & CErasureConfig::GetVolumeErasureMethed()
{
	int k = GetVolumeErasureMethedIndex();
	switch (k)
	{
	case 0: return CErasureMethod::Pseudorandom();
	case 1: return CErasureMethod::HMGIS5Baseline();
	case 2: return CErasureMethod::GOSTP50739();
	case 3: return CErasureMethod::DoD_E();
	case 4: return CErasureMethod::USAF5020();
	case 5: return CErasureMethod::USArmyAR380_19();
	case 6: return CErasureMethod::HMGIS5Enhanced();
	case 7: return CErasureMethod::DoD_EcE();
	case 8: return CErasureMethod::RCMP_TSSIT_OPS_II();
	case 9: return CErasureMethod::Schneier();
	case 10: return CErasureMethod::VSITR();
	case 11: return CErasureMethod::Gutmann();
	default:
		return CErasureMethod::Pseudorandom();
	}
}

int CErasureConfig::GetVolumeErasureMethedIndex()
{
	return GetInteger("volume/mothed", 0);
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

void CErasureConfig::SetFileErasureMothed(int MothedIndex)
{
	SetInteger("file/mothed", MothedIndex);
}

void CErasureConfig::SetRemoveFolder(BOOL value)
{
	SetBoolean("file/removefolder", value);
}

void CErasureConfig::SetVolumeErasureMethed(int MothedIndex)
{
	SetInteger("volume/mothed", MothedIndex);
}

void CErasureConfig::SetSkipSpace(BOOL value)
{
	SetBoolean("volume/skipspace", value);
}

void CErasureConfig::SetSkipTrack(BOOL value)
{
	SetBoolean("volume/skiptrack", value);
}

void CErasureConfig::SetErasureFreeFileSpace(BOOL value)
{
	SetBoolean("volume/filefree", value);
}

void CErasureConfig::SetShutDown(BOOL value)
{
	SetBoolean("volume/shutdown", value);
}

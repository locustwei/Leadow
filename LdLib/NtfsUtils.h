#pragma once

#include <WinIoCtl.h>
#include "VolumeUtils.h"
#include "VolumeInfo.h"

class CNtfsUtils
{
public:
	static DWORD GetNtfsVolumeData(CVolumeInfo* Volume, PNTFS_VOLUME_DATA_BUFFER pVolumeData);
	static DWORD GetNtfsBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData);
};


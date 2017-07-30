#pragma once

#include <WinIoCtl.h>
#include "VolumeUtils.h"
#include "fileinfo.h"

class CNtfsUtils
{
public:
	static DWORD GetNtfsVolumeData(CVolumeInfo* Volume, PNTFS_VOLUME_DATA_BUFFER pVolumeData);
};


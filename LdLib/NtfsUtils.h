#pragma once

#include <WinIoCtl.h>
#include "VolumeUtils.h"

class CNtfsUtils
{
public:
	static DWORD GetNtfsVolumeData(CVolumeInfo& Volume, PNTFS_VOLUME_DATA_BUFFER pVolumeData);
	static DWORD GetBytesPerFileRecord(CVolumeInfo& Volume, PDWORD pOut);
	static DWORD GetMftValidSize(CVolumeInfo& Volume, PUINT64 pOut);
};


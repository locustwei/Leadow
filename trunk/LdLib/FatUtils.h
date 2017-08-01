#pragma once

#include <WinIoCtl.h>
#include "VolumeUtils.h"
#include "VolumeInfo.h"

class CFatUtils
{
public:
	static DWORD GetFatBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData);
};


#pragma once

#include <WinIoCtl.h>
#include "VolumeUtils.h"
#include "VolumeInfo.h"

namespace LeadowLib {
	class CFatUtils
	{
	public:
		static DWORD GetBpbData(CVolumeInfo* Volume, PVOLUME_BPB_DATA pBpbData);
	};
};

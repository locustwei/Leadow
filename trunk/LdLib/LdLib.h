#pragma once


#ifdef _DLL
#define LDLIB_API __declspec(dllexport)
#else
#define LDLIB_API
#endif

#include "classes/LdString.h"
#include "classes/Thread.h"
#include "classes/LdList.h"
#include "classes/LdArray.h"
#include "classes/LdMap.h"
#include "classes/LdTree.h"

#include "ldapp/LdStructs.h"
#include "ldapp/PublicRoutimes.h"
#include "ldapp/LdNamedPipe.h"
#include "ldapp/LdApp.h"
#include "ldapp/LdConfig.h"
#include "ldapp/LdSocket.h"

#include "file/FileInfo.h"
#include "file/FileUtils.h"
#include "volume/VolumeInfo.h"
#include "volume/VolumeUtils.h"
#include "volume/NtfsUtils.h"

#include "utils/DateTimeUtils.h"
#include "utils/DlgGetFileName.h"
#include "utils/HandleUitls.h"
#include "utils/FormatSettings.h"
#include "utils/SHFolders.h"


#define MAKEINT64(low, hi)      ((UINT64)low & 0xffffffff) | (((UINT64)hi & 0xffffffff) << 32)

void DebugOutput(LPCTSTR pstrFormat, ...);
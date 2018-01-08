#pragma once


#ifdef _DLL
#define LDLIB_API __declspec(dllexport)
#else
#define LDLIB_API
#endif

#pragma warning(disable:4996 4091)

//#ifndef __IGernalCallback

//#endif
#include "LdDelegate.h"
#include "classes/LdString.h"
#include "classes/Thread.h"
#include "classes/LdList.h"
#include "classes/LdArray.h"
#include "classes/LdMap.h"
#include "classes/LdTree.h"
#include "classes/DynObject.h"
#include "classes/LdSocket.h"
#include "classes/ShareData.h"

#include "file/FileInfo.h"
#include "file/FileUtils.h"
#include "volume/VolumeInfo.h"
#include "volume/VolumeUtils.h"
#include "volume/NtfsUtils.h"

#include "utils/DateTimeUtils.h"
#include "utils/DlgGetFileName.h"
#include "utils/FormatSettings.h"
#include "utils/SHFolders.h"

#include "PublicRoutimes.h"
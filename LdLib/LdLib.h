#pragma once


#ifdef _DLL
#define LDLIB_API __declspec(dllexport)
#else
#define LDLIB_API
#endif

#ifndef __IGernalCallback

#pragma warning(disable:4996 4091)
/*
通用回掉函数
*/
template <typename T>
interface IGernalCallback
{
	virtual BOOL GernalCallback_Callback(T pData, UINT_PTR Param) = 0;
};


#endif

#include "classes/LdString.h"
#include "classes/Thread.h"
#include "classes/LdList.h"
#include "classes/LdArray.h"
#include "classes/LdMap.h"
#include "classes/LdTree.h"
#include "classes/DynObject.h"
#include "classes/LdSocket.h"

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
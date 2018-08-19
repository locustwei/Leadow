#pragma once


#ifdef _DLL
#define LDLIB_API __declspec(dllexport)
#else
#define LDLIB_API
#endif

#pragma warning(disable:4996 4091)


#include <TCHAR.h>
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


inline void DebugOutput(LPCTSTR pstrFormat, ...)
{
	//#ifdef _DEBUG
	LPTSTR szSprintf;
	va_list argList;
	int nLen;
	va_start(argList, pstrFormat);
	nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (TCHAR*)malloc((nLen + 7) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 7) * sizeof(TCHAR));
	_vsntprintf(szSprintf + 6, nLen + 1, pstrFormat, argList);
	szSprintf[0] = '_';
	szSprintf[1] = '_';
	szSprintf[2] = 'L';
	szSprintf[3] = 'D';
	szSprintf[4] = ' ';
	szSprintf[5] = ' ';
	va_end(argList);
	::OutputDebugString(szSprintf);
	free(szSprintf);
	//#endif
}

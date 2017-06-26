#pragma once

#pragma warning(disable:4996)

#include "LdString.h"
#include "LdStructs.h"
#include "PublicRoutimes.h"
#include "LdNamedPipe.h"
#include "Thread.h"
#include "LdList.h"
#include "LdArray.h"
#include "LdMap.h"
#include "DlgGetFileName.h"

#include "ProcessUtils.h"
#include "FileUtils.h"
#include "HandleUitls.h"
#include "VolumeUtils.h"
#include "RegisterUtils.h"
#include "NtfsUtils.h"
#include "DateTimeUtils.h"

#include "FormatSettings.h"

#define LDLIB_API __declspec(dllexport)

#define PAuthorization PVOID    //Œ™ ⁄»®¡Ù”√

inline void DebugOutput(LPCTSTR pstrFormat, ...)
{
#ifdef _DEBUG
	LPTSTR szSprintf = NULL;
	va_list argList;
	int nLen;
	va_start(argList, pstrFormat);
	nLen = _vsntprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));
	int iRet = _vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
	va_end(argList);
	::OutputDebugString(szSprintf);
	free(szSprintf);
#endif
}
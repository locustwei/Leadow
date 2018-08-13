// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#pragma warning(disable:4091 4477 4996)

#define HIDWORD(a) ((DWORD)((ULONGLONG)(a) >> 32))
#define LODWORD(a) ((DWORD)((ULONGLONG)(a) & 0xFFFFFFFF))

// Windows Í·ÎÄ¼þ: 
#include <Windows.h>
#include <tchar.h>

#include "LdArray.h"
#include "LdString.h"
#include "WJDefines.h"
#include "Thread.h"

using namespace std;
using namespace LeadowLib;

inline void DebugOutput(LPCTSTR pstrFormat, ...)
{
	va_list argList;
	va_start(argList, pstrFormat);


#ifdef _CONSOLE
	vwprintf(pstrFormat, argList);
	//printf(szSprintf);
#else
	LPTSTR szSprintf;
	int nLen = _vstprintf(NULL, 0, pstrFormat, argList);
	szSprintf = (LPTSTR)malloc((nLen + 7) * sizeof(TCHAR));
	ZeroMemory(szSprintf, (nLen + 7) * sizeof(TCHAR));
	_vsntprintf(szSprintf + 6, nLen + 1, pstrFormat, argList);
	szSprintf[0] = 'W';
	szSprintf[1] = 'I';
	szSprintf[2] = 'S';
	szSprintf[3] = 'E';
	szSprintf[4] = ' ';
	szSprintf[5] = ' ';
	if (g_log_file)
	{
		_ftprintf(g_log_file, szSprintf);
		fflush(g_log_file);
	}
	else
		::OutputDebugString(szSprintf);
	free(szSprintf);
#endif
	va_end(argList);
}
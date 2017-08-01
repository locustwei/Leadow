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
#include "LdTree.h"
#include "FileInfo.h"
#include "VolumeInfo.h"

#include "ProcessUtils.h"
#include "FileUtils.h"
#include "HandleUitls.h"
#include "VolumeUtils.h"
#include "RegisterUtils.h"
#include "NtfsUtils.h"
#include "DateTimeUtils.h"

#include "FormatSettings.h"
#include "SHFolders.h"


#define LDLIB_API __declspec(dllexport)

#define MAKEINT64(low, hi)      ((UINT64)low & 0xffff) | (((UINT64)hi & 0xffff) << 32)

#define PAuthorization PVOID    //ÎªÊÚÈ¨ÁôÓÃ

class CLdApp
{
public:
	static boolean Initialize(HINSTANCE hInstance);
	static void MainThreadMessage(MSG& msg);
	static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param);
private:
	HINSTANCE m_Instance;
	DWORD m_ThreadID;
};

void DebugOutput(LPCTSTR pstrFormat, ...);

extern CLdApp* ThisApp;
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

#define PAuthorization PVOID    //Œ™ ⁄»®¡Ù”√

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
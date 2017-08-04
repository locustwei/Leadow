#pragma once

#include "classes/LdString.h"
#include "ldapp/LdStructs.h"
#include "ldapp/PublicRoutimes.h"
#include "ldapp/LdNamedPipe.h"
#include "classes/Thread.h"
#include "classes/LdList.h"
#include "classes/LdArray.h"
#include "classes/LdMap.h"
#include "utils/DlgGetFileName.h"
#include "classes/LdTree.h"
#include "file/FileInfo.h"
#include "volume/VolumeInfo.h"

#include "utils/ProcessUtils.h"
#include "file/FileUtils.h"
#include "utils/HandleUitls.h"
#include "volume/VolumeUtils.h"
#include "utils/RegisterUtils.h"
#include "volume/NtfsUtils.h"
#include "utils/DateTimeUtils.h"

#include "utils/FormatSettings.h"
#include "utils/SHFolders.h"


#define LDLIB_API __declspec(dllexport)

#define MAKEINT64(low, hi)      ((UINT64)low & 0xffff) | (((UINT64)hi & 0xffff) << 32)

#define PAuthorization PVOID    //ÎªÊÚÈ¨ÁôÓÃ

namespace LeadowLib {
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
};
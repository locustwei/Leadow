#include "stdafx.h"
#include "LdLib.h"
#include "SelectProtectFlag.h"
#include "FileProtectDirver.h"
#include "ProcessListView.h"
#include "ProtectMainWnd.h"
#include "ProtectLibrary.h"

class CProtectLibrary : public IProtectLibrary, IEnumSystemHandleCallback
{
public:
	CProtectLibrary()
	{
		m_MainWnd = NULL;
	};
	~CProtectLibrary()
	{
		if (m_MainWnd)
			delete m_MainWnd;
	};
	// Í¨¹ý IProtectLibrary ¼Ì³Ð
	virtual CFramWnd * LibraryUI() override
	{
		if (m_MainWnd == NULL)
		{
			m_MainWnd = new CProtectMainWnd();
		}
		return m_MainWnd;
	};

	virtual DWORD SelectProtectFlag(HWND hParentWnd) override
	{
		return CSelectProtectFlag::SelectFlags(hParentWnd);
	}
	virtual BOOL ProtectFiles(DWORD dwFlags, int nFileCount, LPTSTR * lpFileNames) override
	{
		CFileProtectDirver Driver;
		for (int i = 0; i < nFileCount; i++)
		{
			if (!Driver.ProtectFile(lpFileNames[i], dwFlags))
				return FALSE;
		}
		return TRUE;
	};

	struct CALLBACK_PARAM
	{
		UINT nCount;
		PDWORD pids;
	};

	virtual BOOL SystemHandleCallback(PSYSTEM_HANDLE pHandle, PVOID pParam) override
	{
		struct CALLBACK_PARAM* Param = (struct CALLBACK_PARAM*) pParam;

		if (Param->nCount >= 100)
			return FALSE;
		Param->pids[Param->nCount++] = pHandle->dwProcessId;
		return TRUE;
	};

	virtual BOOL DeleteFiles(LPTSTR lpFileNames, DWORD dwFlags) override
	{
		BOOL Result = DeleteFile(lpFileNames);
		if (Result)
			return TRUE;
		struct CALLBACK_PARAM Param;
		Param.nCount = 0;
		Param.pids = new DWORD[100];
		DWORD ret = CHandleUitls::FindOpenFileHandle(lpFileNames, this, &Param);

		TCHAR* FileExt = _tcsstr(lpFileNames, _T(".exe"));
		if (FileExt && wcslen(FileExt) == 4)
		{
			Param.nCount += CProcessUtils::FindOpenProcess(lpFileNames, Param.pids + Param.nCount);
		}
		FileExt = _tcsstr(lpFileNames, _T(".dll"));
		if (FileExt && wcslen(FileExt) == 4)
		{
			Param.nCount += CProcessUtils::FindOpenModule(lpFileNames, Param.pids + Param.nCount);
		}

		if (Param.nCount)
		{
			if (CProcessListView::KillProcess(NULL, Param.pids, Param.nCount))
			{

			}
		}
		return 0;
	}
private:
	CProtectMainWnd* m_MainWnd;
};

CProtectLibrary* g_Library = NULL;

IProtectLibrary LDLIB_API * API_Init(PAuthorization)
{
	if (g_Library == NULL)
		g_Library = new CProtectLibrary();

	return g_Library;
}

VOID LDLIB_API API_UnInit()
{
	if (g_Library)
	{
		delete g_Library;
		g_Library = NULL;
	}
}


BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		API_UnInit();
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}
	return TRUE;
}
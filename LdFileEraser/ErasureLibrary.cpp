#include "stdafx.h"
#include "FileEraser.h"
#include "ErasureMainWnd.h"
#include "ErasureLibrary.h"

class CErasureLibrary : public IErasureLibrary
{
public:
	CErasureLibrary() 
	{ 
		m_MainWnd = NULL; 
	};
	~CErasureLibrary() 
	{
		if (m_MainWnd)
			delete m_MainWnd;
	};
	virtual CFramWnd* LibraryUI(CPaintManagerUI* pm) override
	{
		if (!m_MainWnd)
			m_MainWnd = new CErasureMainWnd();
		
		assert(m_MainWnd);

		m_MainWnd->BuildXml(_T("erasure\\erasuremain.xml"), pm);

		return m_MainWnd;
	};
private:
	CErasureMainWnd* m_MainWnd;
};

CErasureLibrary* g_Library = NULL;

IErasureLibrary LDLIB_API * API_Init(PAuthorization)
{
	if (!g_Library)
	{
		CLdApp::Initialize(nullptr);
		g_Library = new CErasureLibrary();
	}
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
	case DLL_PROCESS_DETACH:
		API_UnInit();
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		::DisableThreadLibraryCalls((HMODULE)hModule);
		break;
	}
	return TRUE;
}

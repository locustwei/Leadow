#include "stdafx.h"
#include "ui/ErasureMainWnd.h"
#include "ErasureLibrary.h"

class CErasureLibrary;

CErasureLibrary* g_Library = NULL;

class CErasureLibrary : public IErasureLibrary
{
public:

	CErasureLibrary() 
	{ 
		m_MainWnd = NULL; 
		m_Ctrl = nullptr;
	};

	~CErasureLibrary() override
	{
		if (m_MainWnd)
		{
			delete m_MainWnd;
			m_MainWnd = nullptr;
		}
		
		g_Library = nullptr;
	};

	virtual CFramNotifyPump* GetNotifyPump() override
	{
		if (!m_MainWnd)
		{
			m_MainWnd = new CErasureMainWnd();
		}
		assert(m_MainWnd);

		return m_MainWnd;
	};

	TCHAR* UIResorce() override
	{
		return _T("erasure/erasuremain.xml");
	};

	void SetUI(CControlUI* pCtrl) override
	{
		m_Ctrl = pCtrl;
		GetNotifyPump()->AttanchControl(m_Ctrl);
	};

	CControlUI* GetUI() override
	{
		return m_Ctrl;
	};

private:
	CErasureMainWnd* m_MainWnd;
	CControlUI* m_Ctrl;
};


IErasureLibrary LDLIB_API * API_Init(CLdApp* pThisApp)
{
	ThisApp = pThisApp;
	if (!g_Library)
	{
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

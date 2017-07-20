#include "stdafx.h"
#include "Library.h"

typedef PVOID(*Library_Init)(PAuthorization);
typedef VOID(*Library_UnInit)();


PVOID CLdLibray::InitLib(TCHAR * pLibFile, PAuthorization pAut = nullptr)
{
	HMODULE hModule = LoadLibrary(pLibFile);
	if (hModule == NULL)
		return nullptr;
	Library_Init fnInit = (Library_Init)GetProcAddress(hModule, "API_Init");
	if (fnInit == NULL)
		return NULL;
	return fnInit(pAut);
}

IErasureLibrary * CLdLibray::LoadEraserLarary(PAuthorization pAut = nullptr)
{
	return (IErasureLibrary*)InitLib(_T("LdFileEraser_d64.dll"));
}

IErasureLibrary * CLdLibray::LoadProtectLarary(PAuthorization pAut = nullptr)
{
	return (IErasureLibrary*)InitLib(_T("LdFileProtect_d64.dll"));
}

CControlUI * CLdLibray::BuildXml(TCHAR * skinXml, CPaintManagerUI * pm)
{
	if (skinXml == nullptr || _tcslen(skinXml) == 0)
		return nullptr;

	CDialogBuilder builder;
	CControlUI * pControl = builder.Create(skinXml, (UINT)0, NULL, pm);
	_ASSERTE(pControl);

	return pControl;
}

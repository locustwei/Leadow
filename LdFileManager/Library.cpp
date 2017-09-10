#include "stdafx.h"
#include "Library.h"

#ifdef _DEBUG
#ifdef _X64
#define FILE_ERASE_DLL _T("RdErasure_d64.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d64.dll")
#else
#define FILE_ERASE_DLL _T("RdErasure_d32.dll")
#define FILE_PROTECT_DLL _T("LdFileProtect_d32.dll")
#endif
#else
#ifdef _X64
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#endif
#endif


IErasureLibrary * CLdLibray::LoadEraserLarary(CPaintManagerUI* pm, CLdApp* ThisApp)
{
	IErasureLibrary * result = (IErasureLibrary*)CLdDynamicLibrary::InitLib(FILE_ERASE_DLL, ThisApp);

	if(result)
	{
		CControlUI* lui = CLdLibray::BuildXml(result->UIResorce(), pm);
		if (lui)
		{
			result->SetUI(lui);
		}
	}

	return result;
}

IErasureLibrary * CLdLibray::LoadProtectLarary(CLdApp* ThisApp)
{
	return (IErasureLibrary*)CLdDynamicLibrary::InitLib(FILE_PROTECT_DLL, ThisApp);
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

#include "stdafx.h"
#include "Library.h"

IErasureLibrary * CLdLibray::LoadEraserLarary(CPaintManagerUI* pm, PAuthorization pAut)
{
	IErasureLibrary * result = (IErasureLibrary*)CLdDynamicLibrary::InitLib(_T("LdFileEraser_d64.dll"));

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

IErasureLibrary * CLdLibray::LoadProtectLarary(PAuthorization pAut)
{
	return (IErasureLibrary*)CLdDynamicLibrary::InitLib(_T("LdFileProtect_d64.dll"));
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

#pragma once
#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	static IErasureLibrary* LoadEraserLarary(CPaintManagerUI* pm, PAuthorization pAut = nullptr);
	static IErasureLibrary* LoadProtectLarary(PAuthorization pAut = nullptr);
	static CControlUI * BuildXml(TCHAR * skinXml, CPaintManagerUI* pm);
};

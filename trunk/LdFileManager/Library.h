#pragma once
#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	static IErasureLibrary* LoadEraserLarary(PAuthorization pAut = nullptr);
	static IErasureLibrary* LoadProtectLarary(PAuthorization pAut = nullptr);
	static CControlUI * BuildXml(TCHAR * skinXml, CPaintManagerUI* pm);
private:
	static PVOID InitLib(TCHAR * pLibFile, PAuthorization pAut = nullptr);
};

#include "stdafx.h"
#include "Ldbrary.h"
#include "../LdLib.h"

namespace LeadowLib {

	typedef PVOID(*Library_Init)(PAuthorization);
	typedef VOID(*Library_UnInit)();

	PVOID CLdDynamicLibrary::InitLib(TCHAR * pLibFile, PAuthorization pAut)
	{
		HMODULE hModule = LoadLibrary(pLibFile);
		if (hModule == NULL)
			return nullptr;
		Library_Init fnInit = (Library_Init)GetProcAddress(hModule, "API_Init");
		if (fnInit == NULL)
			return NULL;
		return fnInit(pAut);
	}

}
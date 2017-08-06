#pragma once
#include "../LdLib.h"

namespace LeadowLib {

	typedef PVOID(*Library_Init)(PAuthorization);
	typedef VOID(*Library_UnInit)();

	class CLdDynamicLibrary
	{
	public:
		static PVOID InitLib(TCHAR * pLibFile, PAuthorization pAut = nullptr);
	};
};
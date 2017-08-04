#pragma once
#include "../LdLib.h"

namespace LeadowLib {

	typedef PVOID(*Library_Init)(PAuthorization);
	typedef VOID(*Library_UnInit)();

	class CLdLibray
	{
	public:
		static PVOID InitLib(TCHAR * pLibFile, PAuthorization pAut = nullptr);
	};
};
#pragma once
#include "LdStructs.h"

namespace LeadowLib {

	class CLdDynamicLibrary
	{
	public:
		static PVOID InitLib(TCHAR * pLibFile, PAuthorization pAut = nullptr);
	};
};
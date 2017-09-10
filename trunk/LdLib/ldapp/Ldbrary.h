#pragma once

#include "LdApp.h"

namespace LeadowLib {

	class CLdDynamicLibrary
	{
	public:
		static PVOID InitLib(TCHAR * pLibFile, CLdApp* ThisApp);
	};
};
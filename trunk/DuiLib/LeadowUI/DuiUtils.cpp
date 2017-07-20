#include "stdafx.h"
#include "DuiUtils.h"

namespace DuiLib
{
	CDuiUtils::CDuiUtils()
	{
	}
	CDuiUtils::~CDuiUtils()
	{
	}

	CControlUI* CDuiUtils::FindControlByNameProc(CControlUI* ctrl, LPVOID lpName)
	{
		if (ctrl && ctrl->GetName() == (TCHAR*)lpName)
			return ctrl;
		else
			return nullptr;
	}
}
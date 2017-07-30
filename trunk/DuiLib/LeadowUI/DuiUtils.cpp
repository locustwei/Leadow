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

	CControlUI* CDuiUtils::FindControlByClassProc(CControlUI* ctrl, LPVOID lpName)
	{
		if (ctrl && _tcscmp(ctrl->GetClass(), (TCHAR*)lpName) == 0)
			return ctrl;
		else
			return nullptr;
	}
}
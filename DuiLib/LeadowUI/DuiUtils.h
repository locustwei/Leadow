#pragma once

namespace DuiLib
{
	class DUILIB_API CDuiUtils
	{
	public:
		CDuiUtils();
		~CDuiUtils();
		static CControlUI* CALLBACK FindControlByNameProc(CControlUI* ctrl, LPVOID lpName);

	};

}
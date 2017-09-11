#pragma once

#include "../classes/LdString.h"
#include "../utils/HandleUitls.h"

namespace LeadowLib {

	class CLdApp
	{
	public:
		CLdApp();
		CLdString& GetInstallPath();
		CLdString& GetAppDataPath();

		static CLdApp* ThisApp;

		static BOOL Initialize(HINSTANCE hInstance);
		static void MainThreadMessage(MSG& msg);
		static BOOL Send2MainThread(IGernalCallback<LPVOID>* callback, UINT_PTR Param);
	private:
		HINSTANCE m_Instance;
		DWORD m_ThreadID;
		CLdString m_InstallPath;
		CLdString m_AppDataPath;
	};

	void DebugOutput(LPCTSTR pstrFormat, ...);

};
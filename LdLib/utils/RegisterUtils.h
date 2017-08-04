#pragma once

namespace LeadowLib {
	class CRegisterKey
	{
	public:
		CRegisterKey(HKEY hRoot, LPTSTR lpKey);
		~CRegisterKey();

		CRegisterKey OpenSubKey(LPTSTR lpKey);
		DWORD DeleteRegKey(LPTSTR lpKey);
		DWORD DeleteValue(TCHAR* szValue);
	private:
		HKEY m_Root;
		HKEY m_hKey;
	};


	class CRegisterUtils
	{
	public:
		static CRegisterKey CurrentUser;
		static CRegisterKey ClassRoot;
		static CRegisterKey LocalMachine;

		static DWORD RegAccessMode(TCHAR* szKey, BOOL bReadonly);
		static HKEY OpenKey(HKEY hRoot, LPTSTR lpKey, BOOL bReadonly = FALSE);
		static DWORD DeleteRegKey(HKEY hRoot, TCHAR* szKey);
		static DWORD DeleteRegValue(HKEY hRoot, TCHAR* szKey, TCHAR* szValue);
	};

};
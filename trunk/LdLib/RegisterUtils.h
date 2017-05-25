#pragma once

class CRegisterKey
{
public:
	CRegisterKey(HKEY hRoot, LPTSTR lpKey);
	~CRegisterKey();

	CRegisterKey OpenSubKey(LPTSTR lpKey);
	DWORD DeleteRegKey(LPTSTR lpKey);
	DWORD DeleteValue(LPCTSTR szValue);
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

	static DWORD RegAccessMode(LPCTSTR szKey, BOOL bReadonly);
	static HKEY OpenKey(HKEY hRoot, LPTSTR lpKey, BOOL bReadonly = FALSE);
	static DWORD DeleteRegKey(HKEY hRoot, LPCTSTR szKey);
	static DWORD DeleteRegValue(HKEY hRoot, LPCTSTR szKey, LPCTSTR szValue);
};


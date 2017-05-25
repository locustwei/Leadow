#include "stdafx.h"
#include "RegisterUtils.h"
#include "PublicRoutimes.h"
#include "LdString.h"
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

CRegisterKey CRegisterUtils::CurrentUser(HKEY_CURRENT_USER, NULL);
CRegisterKey CRegisterUtils::ClassRoot(HKEY_CLASSES_ROOT, NULL);
CRegisterKey CRegisterUtils::LocalMachine(HKEY_LOCAL_MACHINE, NULL);

CRegisterKey::CRegisterKey(HKEY hRoot, LPTSTR lpKey)
{
	m_Root = hRoot;
	if (lpKey)
		m_hKey = CRegisterUtils::OpenKey(hRoot, lpKey);
	else
		m_hKey = NULL;
}

CRegisterKey::~CRegisterKey()
{
	if (m_hKey)
		RegCloseKey(m_hKey);
}

CRegisterKey CRegisterKey::OpenSubKey(LPTSTR lpKey)
{
	return CRegisterKey(m_Root, lpKey);
}

DWORD CRegisterKey::DeleteRegKey(LPTSTR lpKey)
{
	return SHDeleteKey(m_hKey, lpKey);
}

DWORD CRegisterKey::DeleteValue(LPCTSTR szValue)
{
	return RegDeleteValue(m_hKey, szValue);
}

inline DWORD CRegisterUtils::RegAccessMode(LPCTSTR szKey, BOOL bReadonly)  //设置注册表访问方式。
{
	DWORD result = 0;
	if (bReadonly)
		result |= KEY_READ;
	else
		result |= KEY_ALL_ACCESS;
	if (IsWindow64()) {
		CLdString cszTmp(szKey);
		cszTmp.MakeLower();
		if ((cszTmp.Find(_T("wow6432node")) >= 0) && IsVista64())
			result |= KEY_WOW64_32KEY;
		else
			result |= KEY_WOW64_64KEY;
	}
	
	return result;
}

HKEY CRegisterUtils::OpenKey(HKEY hRoot, LPTSTR lpKey, BOOL bReadonly)
{
	DWORD dwDesired = RegAccessMode(lpKey, bReadonly);
	HKEY Result = NULL;
	RegOpenKeyEx(hRoot, lpKey, NULL, dwDesired, &Result);
	return Result;
}

DWORD CRegisterUtils::DeleteRegKey(HKEY hRoot, LPCTSTR szKey)
{
	CLdString cszKey;
	CLdString cszDelKey(szKey);

	int nIdx = cszDelKey.ReverseFind('\\');
	if (nIdx != -1) {
		cszKey = cszDelKey.Left(nIdx);
		cszDelKey = cszDelKey.Right(cszDelKey.GetLength() - nIdx - 1);
	}

	DWORD dwDesired = RegAccessMode(szKey, false);
	HKEY hKey;
	DWORD Result = RegOpenKeyEx(hRoot, cszKey, NULL, dwDesired, &hKey);
	if(Result == ERROR_SUCCESS) 
	{
		Result = SHDeleteKey(hKey, cszDelKey);
		RegCloseKey(hKey);
	}
	return Result;
}

DWORD CRegisterUtils::DeleteRegValue(HKEY hRoot, LPCTSTR szKey, LPCTSTR szValue)
{
	DWORD dwDesired = RegAccessMode(szKey, false);
	HKEY hKey;
	DWORD Result = RegOpenKeyEx(hRoot, szKey, NULL, dwDesired, &hKey);
	if(Result == ERROR_SUCCESS) 
	{
		Result = RegDeleteValue(hKey, szValue);
		RegCloseKey(hKey);
	}
	return Result;
}

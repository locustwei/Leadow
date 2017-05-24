#include "stdafx.h"
#include "RegisterUtils.h"
#include "PublicRoutimes.h"
#include "LdString.h"

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

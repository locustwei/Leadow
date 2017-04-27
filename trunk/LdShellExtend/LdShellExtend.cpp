// LdShellExtend.cpp : DLL 导出的实现。


#include "stdafx.h"
#include "resource.h"
#include "LdShellExtend_i.h"
#include "dllmain.h"

bool RegContexMenu()
{
	ATL::CRegKey reg;
	LONG    lRet = ERROR_SUCCESS;
	
	do 
	{
		lRet = reg.Open(HKEY_LOCAL_MACHINE, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), KEY_SET_VALUE );
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetStringValue (GUID_STR, REG_KEYNAME);
		if ( ERROR_SUCCESS != lRet )
			break;

		lRet = reg.Create( HKEY_CLASSES_ROOT, _T("*\\shellex\\ContextMenuHandlers\\" REG_KEYNAME));
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetStringValue (NULL, GUID_STR);
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetDWORDValue(REG_TYPE_VALUE, CONTEX_MENU_ALLFILE);
		if ( ERROR_SUCCESS != lRet )
			break;

		lRet = reg.Create( HKEY_CLASSES_ROOT, _T("Folder\\shellex\\ContextMenuHandlers\\" REG_KEYNAME));
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetStringValue (NULL, GUID_STR);
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetDWORDValue(REG_TYPE_VALUE, CONTEX_MENU_FOLDER);
		if ( ERROR_SUCCESS != lRet )
			break;

		lRet = reg.Create ( HKEY_CLASSES_ROOT, _T("Directory\\Background\\shellex\\ContextMenuHandlers\\" REG_KEYNAME));
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetStringValue (NULL, GUID_STR);
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetDWORDValue(REG_TYPE_VALUE, CONTEX_MENU_DIR_BKG);
		if ( ERROR_SUCCESS != lRet )
			break;

		lRet = reg.Create ( HKEY_CLASSES_ROOT, _T("Drive\\shellex\\ContextMenuHandlers\\" REG_KEYNAME));
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetStringValue (NULL, GUID_STR);
		if ( ERROR_SUCCESS != lRet )
			break;
		lRet = reg.SetDWORDValue(REG_TYPE_VALUE, CONTEX_MENU_DRIVE);
		if ( ERROR_SUCCESS != lRet )
			break;

	} while (false);

	return lRet == ERROR_SUCCESS;
}

void UnRegContextMenu()
{
	ATL::CRegKey reg;
	LONG    lRet;

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("*\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey ( REG_KEYNAME);
	if ( ERROR_SUCCESS != lRet )
		return ;

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("Folder\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (REG_KEYNAME);

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("Directory\\Background\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (REG_KEYNAME);

	lRet = reg.Create ( HKEY_CLASSES_ROOT,
		_T("Drive\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (REG_KEYNAME);
}

// 用于确定 DLL 是否可由 OLE 卸载。
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

// 返回一个类工厂以创建所请求类型的对象。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - 在系统注册表中添加项。
STDAPI DllRegisterServer(void)
{
	if(!RegContexMenu())
		return E_ACCESSDENIED;
	// 注册对象、类型库和类型库中的所有接口
	HRESULT hr = _AtlModule.DllRegisterServer();
	return hr;
}

// DllUnregisterServer - 在系统注册表中移除项。
STDAPI DllUnregisterServer(void)
{
	UnRegContextMenu();

	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - 按用户和计算机在系统注册表中逐一添加/移除项。
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != NULL)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{	
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}



// LdShellExtend.cpp : DLL ������ʵ�֡�


#include "stdafx.h"
#include "resource.h"
#include "LdShellExtend_i.h"
#include "dllmain.h"

#define KEYNAME _T("Leadow Shell Extend")
#define GUID_STR _T("{7C4429E3-0593-45DC-BF37-F2CBCEFB27F2}")

bool RegContexMenu()
{
	ATL::CRegKey reg;
	LONG    lRet;
	
	lRet = reg.Open ( HKEY_LOCAL_MACHINE,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),
		KEY_SET_VALUE );
	if ( ERROR_SUCCESS != lRet )
		return false;
	lRet = reg.SetStringValue ( KEYNAME, GUID_STR );
	if ( ERROR_SUCCESS != lRet )
		return false;

	lRet = reg.Create( HKEY_CLASSES_ROOT,
		_T("*\\shellex\\ContextMenuHandlers\\" KEYNAME));
	if ( ERROR_SUCCESS != lRet )
		return false;
	lRet = reg.SetStringValue (GUID_STR, NULL);
	if ( ERROR_SUCCESS != lRet )
		return false;

	lRet = reg.Create ( HKEY_CLASSES_ROOT,
		_T("Folder\\shellex\\ContextMenuHandlers\\" KEYNAME));
	if ( ERROR_SUCCESS != lRet )
		return false;
	lRet = reg.SetStringValue (GUID_STR, NULL);
	if ( ERROR_SUCCESS != lRet )
		return false;

	lRet = reg.Create ( HKEY_CLASSES_ROOT,
		_T("\\Directory\\Background\\shellex\\ContextMenuHandlers\\" KEYNAME));
	if ( ERROR_SUCCESS != lRet )
		return false;
	lRet = reg.SetStringValue (GUID_STR, NULL);
	if ( ERROR_SUCCESS != lRet )
		return false;

	lRet = reg.Create ( HKEY_CLASSES_ROOT,
		_T("\\Drive\\shellex\\ContextMenuHandlers\\" KEYNAME));
	if ( ERROR_SUCCESS != lRet )
		return false;
	lRet = reg.SetStringValue (GUID_STR, NULL);
	if ( ERROR_SUCCESS != lRet )
		return false;

	return true;
}

void UnRegContextMenu()
{
	ATL::CRegKey reg;
	LONG    lRet;

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("*\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey ( KEYNAME);
	if ( ERROR_SUCCESS != lRet )
		return ;

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("Folder\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (KEYNAME);

	lRet = reg.Open ( HKEY_CLASSES_ROOT,
		_T("\\Directory\\Background\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (KEYNAME);

	lRet = reg.Create ( HKEY_CLASSES_ROOT,
		_T("\\Drive\\shellex\\ContextMenuHandlers"));
	if ( ERROR_SUCCESS != lRet )
		return ;
	lRet = reg.DeleteSubKey (KEYNAME);
}

// ����ȷ�� DLL �Ƿ���� OLE ж�ء�
STDAPI DllCanUnloadNow(void)
{
	return _AtlModule.DllCanUnloadNow();
}

// ����һ���๤���Դ������������͵Ķ���
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

// DllRegisterServer - ��ϵͳע���������
STDAPI DllRegisterServer(void)
{
	if(!RegContexMenu())
		return E_ACCESSDENIED;
	// ע��������Ϳ�����Ϳ��е����нӿ�
	HRESULT hr = _AtlModule.DllRegisterServer();
		return hr;
}

// DllUnregisterServer - ��ϵͳע������Ƴ��
STDAPI DllUnregisterServer(void)
{
	UnRegContextMenu();

	HRESULT hr = _AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - ���û��ͼ������ϵͳע�������һ���/�Ƴ��
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



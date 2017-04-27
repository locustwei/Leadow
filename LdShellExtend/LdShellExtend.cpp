// LdShellExtend.cpp : DLL ������ʵ�֡�


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



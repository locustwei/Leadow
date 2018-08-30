#include "stdafx.h"
#include <tchar.h>
#include "LdPlugin.h"

INT_PTR CPluginManager::EnumFile_Callback(PVOID data, UINT_PTR param)
{
	PWIN32_FIND_DATA pData = (PWIN32_FIND_DATA)data;

	CLdString filename = m_Plug_path;
	if (filename[filename.GetLength() - 1] != '\\')
		filename += '\\';
	filename += pData->cFileName;

	HMODULE hmodule = LoadLibrary(filename);
	if (hmodule != nullptr)
	{
		_API_Register fn = (_API_Register)GetProcAddress(hmodule, "API_Register");
		if (fn != nullptr)
		{
			PLUGIN_PROPERTY prpperty = fn();
			if (prpperty.func)
			{
				m_Plugins.Put(filename, prpperty);
			}
		}
		FreeLibrary(hmodule);
	}
	return 1;
}

CPluginManager::CPluginManager(const TCHAR* path)
{
	m_Plug_path = path;
}

CPluginManager::~CPluginManager()
{
}

void CPluginManager::FindPlugin(PLUGIN_USAGE usage, CLdArray<PLUGIN_PROPERTY>* out_result)
{
	if (m_Plugins.GetCount() == 0)
		LoadAllPlugins();
	for (int i = 0; i<m_Plugins.GetCount(); i++)
	{
		PLUGIN_PROPERTY property = m_Plugins.ValueOf(i);
		if (property.func & usage)
			out_result->Add(property);
	}
}

IPluginInterface* CPluginManager::LoadPlugin(CLdApp* app, TCHAR* plugid)
{
	if (m_Plugins.GetCount() == 0)
		LoadAllPlugins();

	CLdString filename;

	for (int i = 0; i<m_Plugins.GetCount(); i++)
	{
		PLUGIN_PROPERTY property = m_Plugins.ValueOf(i);
		if (_tcscmp(property.id, plugid) == 0)
		{
			filename = m_Plugins.KeyOf(i);
			break;
		}
	}
	if (!filename.IsEmpty())
	{
		HMODULE hmodule = LoadLibrary(filename);
		if (hmodule != nullptr)
		{
			_API_Init fn = (_API_Init)GetProcAddress(hmodule, "API_Init");
			if (fn != nullptr)
			{
				return fn(app);
			}
			else
				FreeLibrary(hmodule);
		}
	}
	DebugOutput(L"CPluginManager::LoadPlugin not found %s ", plugid);
	return nullptr;
}

void CPluginManager::LoadAllPlugins()
{
	CFileUtils::EnumFiles((TCHAR*)m_Plug_path.GetData(), _T("*.dll"), CLdMethodDelegate::MakeDelegate(this, &CPluginManager::EnumFile_Callback), (UINT_PTR)& m_Plugins);
}

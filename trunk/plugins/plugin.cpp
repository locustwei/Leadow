#include "stdafx.h"
#include "plugin.h"


struct ENUM_FILE_PARAM
{
	const TCHAR* path;
	PLUGIN_USAGE usage;
	CLdMap<CLdString, PLUGIN_PROPERTY>* out_result;
};

INT_PTR EnumFile_Callback(PVOID data, UINT_PTR param)
{
	PWIN32_FIND_DATA pData = (PWIN32_FIND_DATA)data;
	struct ENUM_FILE_PARAM* pParam = (struct ENUM_FILE_PARAM*)param;
	
	CLdString filename = (TCHAR*)pParam->path;
	if (filename[filename.GetLength() - 1] != '\\')
		filename += '\\';
	filename += pData->cFileName;

	HMODULE hmodule = LoadLibrary(filename);
	if(hmodule != nullptr)
	{
		_API_Register fn = (_API_Register)GetProcAddress(hmodule, "");
		if (fn != nullptr)
		{
			PLUGIN_PROPERTY prpperty = fn();
			if(prpperty.func & pParam->usage)
			{
				pParam->out_result->Put(filename, prpperty);
			}
		}
		FreeLibrary(hmodule);
	}
	return 1;
}

void CPluginManager::FindPlugin(const TCHAR* path, PLUGIN_USAGE usage, CLdMap<CLdString, PLUGIN_PROPERTY>* out_result)
{
	struct ENUM_FILE_PARAM param;
	param.path = path;
	param.usage = usage;
	param.out_result = out_result;

	CFileUtils::EnumFiles((TCHAR*)path, _T("*.dll"), CMethodDelegate::MakeDelegate(EnumFile_Callback), (UINT_PTR)&param);

}
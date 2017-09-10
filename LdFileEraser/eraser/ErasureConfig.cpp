#include "stdafx.h"
#include "ErasureConfig.h"
#include <locale>
#include <codecvt>

using namespace std;

CErasureConfig::CErasureConfig()
{
	CLdString& appPath = ThisApp->GetAppDataPath();
	CLdString cf = appPath;
	cf += _T("rderasure.cng");

	char* lpMultiByteStr = new char[(cf.GetLength()+1)*sizeof(TCHAR)];

	WideCharToMultiByte(CP_ACP, NULL, cf.GetData(), -1, lpMultiByteStr, cf.GetLength()*sizeof(TCHAR), NULL, FALSE);

	Value json;
	json.loadFromFile(lpMultiByteStr);
	delete lpMultiByteStr;
}


CErasureConfig::~CErasureConfig()
{
}

#include "stdafx.h"
#include "ErasureConfig.h"
#include "../Jsonlib/JsonBox.h"

using namespace std;


CErasureConfig::CErasureConfig()
	:m_Config()
	,m_ConfigFileName()
{
	CLdString& appPath = CLdApp::ThisApp->GetAppDataPath();
	CLdString cf = appPath;
	cf += _T("rderasure.cng");

	char* lpMultiByteStr = new char[(cf.GetLength()+1)*sizeof(TCHAR)];

	WideCharToMultiByte(CP_ACP, NULL, cf.GetData(), -1, lpMultiByteStr, cf.GetLength()*sizeof(TCHAR), NULL, FALSE);

	JsonBox::Value json;
	json.loadFromFile(lpMultiByteStr);
	delete lpMultiByteStr;
}


CErasureConfig::~CErasureConfig()
{
}

BOOL CErasureConfig::LoadConfig()
{
	m_Config.loadFromFile(m_ConfigFileName);
	return true;
}

BOOL CErasureConfig::GetBoolean(TCHAR* Path)
{
	CLdStringA string;
	string = ((wchar_t*)Path);
	for(int i=0; i<string.GetLength(); i++)
	{
		if (string.GetData()[i] == '\\')
			string.GetData()[i] = '\0';
	}
	JsonBox::Object obj = m_Config.getObject();
	JsonBox::Value val = obj[""];
	val.getObject();
	return true;
}

double CErasureConfig::GetDouble(TCHAR * Path)
{
	return 0.0;
}

float CErasureConfig::GetFloat(TCHAR * Path)
{
	return 0.0f;
}

int CErasureConfig::GetInteger(TCHAR * Path)
{
	return 0;
}

CLdString CErasureConfig::GetString(TCHAR * Path)
{
	return CLdString();
}

VOID CErasureConfig::SetBoolean(TCHAR * Path, BOOL Value)
{
	return VOID();
}

VOID CErasureConfig::SetDouble(TCHAR * Path, double Value)
{
	return VOID();
}

VOID CErasureConfig::SetFloat(TCHAR * Path, float Value)
{
	return VOID();
}

VOID CErasureConfig::SetInteger(TCHAR * Path, int Value)
{
	return VOID();
}

VOID CErasureConfig::SetString(TCHAR * Path, TCHAR * Value)
{
	return VOID();
}

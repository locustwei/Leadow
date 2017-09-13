#include "stdafx.h"
#include "ErasureConfig.h"
#include "../Jsonlib/JsonBox.h"

using namespace std;


CErasureConfig::CErasureConfig()
	:m_Config()
	,m_ConfigFileName("file.json")
{
	/*CLdString& appPath = CLdApp::ThisApp->GetAppDataPath();
	CLdString cf = appPath;
	cf += _T("rderasure.cng");

	char* lpMultiByteStr = new char[(cf.GetLength()+1)*sizeof(TCHAR)];

	WideCharToMultiByte(CP_ACP, NULL, cf.GetData(), -1, lpMultiByteStr, cf.GetLength()*sizeof(TCHAR), NULL, FALSE);

	JsonBox::Value json;
	json.loadFromFile(lpMultiByteStr);
	delete lpMultiByteStr;*/
	
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
	JsonBox::Value val = GetConfigObject(Path);

	return val.getBoolean();
}

double CErasureConfig::GetDouble(TCHAR * Path)
{
	return GetConfigObject(Path).getDouble();
}

float CErasureConfig::GetFloat(TCHAR * Path)
{
	return GetConfigObject(Path).getFloat();
}

int CErasureConfig::GetInteger(TCHAR * Path)
{
	return GetConfigObject(Path).getInteger();
}

CLdString CErasureConfig::GetString(TCHAR * Path)
{
	JsonBox::Value val = GetConfigObject(Path);
	string s = val.getString();
	CLdString result;
	result = ((char*)s.c_str());
	return result;
}

VOID CErasureConfig::SetBoolean(TCHAR * Path, BOOL Value)
{
	GetConfigObject(Path).setBoolean(Value);
}

VOID CErasureConfig::SetDouble(TCHAR * Path, double Value)
{
	GetConfigObject(Path).setDouble(Value);
}

VOID CErasureConfig::SetFloat(TCHAR * Path, float Value)
{
	GetConfigObject(Path).setDouble(Value);
}

VOID CErasureConfig::SetInteger(TCHAR * Path, int Value)
{
	GetConfigObject(Path).setInteger(Value);
}

VOID CErasureConfig::SetString(TCHAR * Path, TCHAR * Value)
{
	CLdStringA s;
	s = Value;
	JsonBox::Value val = GetConfigObject(Path);
	val.setString(s.GetData());
}

JsonBox::Value CErasureConfig::GetConfigObject(TCHAR* Path)
{
	CLdStringA string;
	string = ((wchar_t*)Path);

	int len = string.GetLength();

	for (int i = 0; i<string.GetLength(); i++)
	{
		if (string.GetData()[i] == '\\')
			string.GetData()[i] = '\0';
	}

	size_t k = 0;
	JsonBox::Value val = m_Config;

	while (k < len)
	{
		char* p = string.GetData() + k;
		k += strlen(p) + 1;
		JsonBox::Object obj = val.getObject();
		val = obj[p];
	}
	return val;
}

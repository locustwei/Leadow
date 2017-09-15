#include "stdafx.h"
#include "LdConfig.h"
#include "../file/FileUtils.h"
#include <Shlwapi.h>

//#pragma warning(disable:4800)

namespace LeadowLib {

	CLdConfig::CLdConfig()
		:m_Config()
		, m_ConfigFileName()
	{

	}


	CLdConfig::~CLdConfig()
	{
	}

	BOOL CLdConfig::LoadConfig()
	{
		try {
			if(!PathFileExistsA(m_ConfigFileName))
			{
				CLdString tmp = (char*)m_ConfigFileName;
				CLdString path((UINT)MAX_PATH);
				CFileUtils::ExtractFilePath(tmp, path.GetData());
				CFileUtils::ForceDirectories(path);
			}
			m_Config.loadFromFile(m_ConfigFileName);
			return TRUE;
		}catch(...)
		{
			return FALSE;
		}
	}

	BOOL CLdConfig::SaveConfig()
	{
		try {
			m_Config.writeToFile(m_ConfigFileName);
			return TRUE;
		}
		catch (...)
		{
			return FALSE;
		}
	}

	BOOL CLdConfig::GetBoolean(TCHAR* Path)
	{
		JsonBox::Value val = GetConfigObject(Path);

		return val.getBoolean();
	}

	double CLdConfig::GetDouble(TCHAR * Path)
	{
		return GetConfigObject(Path).getDouble();
	}

	float CLdConfig::GetFloat(TCHAR * Path)
	{
		return GetConfigObject(Path).getFloat();
	}

	int CLdConfig::GetInteger(TCHAR * Path)
	{
		return GetConfigObject(Path).getInteger();
	}

	CLdString CLdConfig::GetString(TCHAR * Path)
	{
		JsonBox::Value val = GetConfigObject(Path);
		CLdString result;
		result = ((char*)val.getString().c_str());
		return result;
	}

	BOOL CLdConfig::GetBoolean(char* Path, BOOL def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetBoolean(def == TRUE) ;
	}

	double CLdConfig::GetDouble(char* Path, double def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetDouble(def);
	}

	float CLdConfig::GetFloat(char* Path, float def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetFloat(def);
	}

	int CLdConfig::GetInteger(char* Path, int def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetInteger(def);
	}

	CLdString CLdConfig::GetString(char* Path, TCHAR* def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty())
			result = def;
		return result;
	}

	VOID CLdConfig::SetBoolean(char * Path, BOOL Value)
	{
		GetConfigObject(Path).setBoolean(Value==TRUE);
	}

	VOID CLdConfig::SetDouble(char * Path, double Value)
	{
		GetConfigObject(Path).setDouble(Value);
	}

	VOID CLdConfig::SetFloat(char * Path, float Value)
	{
		GetConfigObject(Path).setDouble(Value);
	}

	VOID CLdConfig::SetInteger(char * Path, int Value)
	{
		GetConfigObject(Path).setInteger(Value);
	}

	VOID CLdConfig::SetString(char * Path, TCHAR * Value)
	{
		CLdStringA s;
		s = Value;
		JsonBox::Value val = GetConfigObject(Path);
		val.setString(s.GetData());
	}

	JsonBox::Value CLdConfig::GetConfigObject(CLdStringA string)
	{
		int len = string.GetLength();

		for (int i = 0; i < string.GetLength(); i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
				string.GetData()[i] = '\0';
		}

		int k = 0;
		JsonBox::Value val = m_Config;

		while (k < len)
		{
			char* p = string.GetData() + k;
			k += strlen(p) + 1;
			JsonBox::Object obj = val.getObject();
			if(obj._Isnil())
			val = obj[p];
		}
		return val;
	}
}
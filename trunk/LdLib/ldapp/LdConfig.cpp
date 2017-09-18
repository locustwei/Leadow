#include "stdafx.h"
#include "LdConfig.h"
#include "../file/FileUtils.h"
#include <Shlwapi.h>
#include "../classes/LdMap.h"

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
				if (CFileUtils::ForceDirectories(path) != 0)
					return FALSE;
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

	void CLdConfig::operator=(CLdConfig & source)
	{
		m_Config = source.m_Config;
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
		GetConfigObject(Path, JsonBox::Value::BOOLEAN, &Value);
	}

	VOID CLdConfig::SetDouble(char * Path, double Value)
	{
		GetConfigObject(Path, JsonBox::Value::DOUBLE, &Value);
	}

	VOID CLdConfig::SetFloat(char * Path, float Value)
	{
		GetConfigObject(Path, JsonBox::Value::DOUBLE, &Value);
	}

	VOID CLdConfig::SetInteger(char * Path, int Value)
	{
		GetConfigObject(Path, JsonBox::Value::INTEGER, &Value);
	}

	VOID CLdConfig::SetString(char * Path, TCHAR * Value)
	{
		CLdStringA s;
		s = Value;
		GetConfigObject(Path, JsonBox::Value::STRING, s.GetData());
	}

	JsonBox::Value CLdConfig::GetConfigObject(CLdStringA string, JsonBox::Value::Type type, PVOID pValue)
	{
		int len = string.GetLength();
		CLdMap<char*, JsonBox::Value> objs;
		char* p = string.GetData();
		JsonBox::Value parent = m_Config;
		JsonBox::Value value = JsonBox::Value::NULL_VALUE;

		for (int i = 0; i < len; i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
			{
				string.GetData()[i] = '\0';
				value = parent[p];
				if (type != JsonBox::Value::NULL_VALUE)  
				{//需要添加不存在的对象,把路径保存下来.
					objs.Put(p, parent);
				}
				else if (value.isNull())
					break;
				parent = value;
				p = string.GetData() + i + 1;
			}
		}
		objs.Put(p, parent);
		value = parent[p];

		if(type!=JsonBox::Value::NULL_VALUE && pValue!=nullptr)
		{
			switch(type)
			{
			case JsonBox::Value::Type::STRING:
				value.setString((char*)pValue);
				break;
			case JsonBox::Value::Type::BOOLEAN:
				value.setBoolean((*(PBOOL)pValue)==TRUE);
				break;
			case JsonBox::Value::Type::DOUBLE:
				value.setDouble(*(double*)pValue);
				break;
			case JsonBox::Value::Type::INTEGER:
				value.setInteger(*(int*)pValue);
				break;
			default:
				return value;
			}

			JsonBox::Value v = value;

			JsonBox::Value *pItem = nullptr;
			for(int i=objs.GetCount()-1;i>=0;i--)
			{
				char** pkey = objs.GetAt(i, &pItem);
				(*pItem)[*pkey] = v;
				v = *pItem;
			}
			if(pItem)
				m_Config = *pItem;
		}

		return value;
	}
}
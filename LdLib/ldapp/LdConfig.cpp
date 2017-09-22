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

	int CLdConfig::GetArrayCount(CLdStringA Path)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return 0;
		else
		{
			return (int)val.getArray().size();
		}
	}

	BOOL CLdConfig::GetArrayBoolean(CLdStringA Path, int index, BOOL def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return def;
		else
		{
			JsonBox::Array a = val.getArray();
			if (a.size() <= index)
				return def;
			else
				return a[index].tryGetBoolean(def == TRUE);
		}
	}

	double CLdConfig::GetArrayDouble(CLdStringA Path, int index, double def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return def;
		else
		{
			JsonBox::Array a = val.getArray();
			if (a.size() <= index)
				return def;
			else
				return a[index].tryGetDouble(def);
		}
	}

	float CLdConfig::GetArrayFloat(CLdStringA Path, int index, float def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return def;
		else
		{
			JsonBox::Array a = val.getArray();
			if (a.size() <= index)
				return def;
			else
				return a[index].tryGetFloat(def);
		}
	}

	int CLdConfig::GetArrayInteger(CLdStringA Path, int index, int def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return def;
		else
		{
			JsonBox::Array a = val.getArray();
			if (a.size() <= index)
				return def;
			else
				return a[index].tryGetInteger(def);
		}
	}

	CLdString CLdConfig::GetArrayString(CLdStringA Path, int index, TCHAR * def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return def;
		else
		{
			JsonBox::Array a = val.getArray();
			if (a.size() <= index)
				return def;
			else
			{
				CLdString result;
				result = val.tryGetString("").c_str();
				if (result.IsEmpty())
					result = def;
				return result;
			}
		}
	}

	VOID CLdConfig::SetArrayBoolean(CLdStringA Path, int index, BOOL Value)
	{
		GetConfigObject(Path, index, JsonBox::Value::BOOLEAN, &Value);
	}

	BOOL CLdConfig::GetBoolean(CLdStringA Path, BOOL def)
	{
		JsonBox::Value val = GetConfigObject(Path);

		return val.tryGetBoolean(def == TRUE);
	}

	double CLdConfig::GetDouble(CLdStringA Path, double def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetDouble(def);
	}

	float CLdConfig::GetFloat(CLdStringA Path, float def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetFloat(def);
	}

	int CLdConfig::GetInteger(CLdStringA Path, int def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.tryGetInteger(def);
	}

	CLdString CLdConfig::GetString(CLdStringA Path, TCHAR* def)
	{
		JsonBox::Value val = GetConfigObject(Path);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty())
			result = def;
		return result;
	}

	VOID CLdConfig::SetBoolean(CLdStringA Path, BOOL Value)
	{
		GetConfigObject(Path, 0, JsonBox::Value::BOOLEAN, &Value);
	}

	VOID CLdConfig::SetDouble(CLdStringA Path, double Value)
	{
		GetConfigObject(Path, 0, JsonBox::Value::DOUBLE, &Value);
	}

	VOID CLdConfig::SetFloat(CLdStringA Path, float Value)
	{
		GetConfigObject(Path, 0, JsonBox::Value::DOUBLE, &Value);
	}

	VOID CLdConfig::SetInteger(CLdStringA Path, int Value)
	{
		GetConfigObject(Path, 0, JsonBox::Value::INTEGER, &Value);
	}

	VOID CLdConfig::SetString(CLdStringA Path, TCHAR * Value)
	{
		CLdStringA s;
		s = Value;
		GetConfigObject(Path, 0, JsonBox::Value::STRING, s.GetData());
	}

	JsonBox::Value CLdConfig::GetConfigObject(CLdStringA string, int index, JsonBox::Value::Type type, PVOID pValue)
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
			if (type == JsonBox::Value::ARRAY)
				value = value[index];

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
			case JsonBox::Value::ARRAY:
				value[index] = "";
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
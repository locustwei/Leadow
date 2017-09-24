#include "stdafx.h"
#include "LdConfig.h"
#include "../file/FileUtils.h"
#include <Shlwapi.h>
#include "../classes/LdMap.h"
#include <sstream>

#pragma warning(disable:4018)
#pragma comment(lib,"Shlwapi.lib")

using namespace std;

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

	int CLdConfig::GetDataType(CLdStringA Path)
	{
		JsonBox::Value val = GetConfigObject(Path);
		return val.getType();
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

	BOOL CLdConfig::GetBoolean(CLdStringA Path, BOOL def, int index)
	{
		JsonBox::Value val = GetConfigObject(Path, index);
		return val.tryGetBoolean(def == TRUE);
	}

	double CLdConfig::GetDouble(CLdStringA Path, double def, int index)
	{
		JsonBox::Value val = GetConfigObject(Path, index);
		return val.tryGetDouble(def);
	}

	float CLdConfig::GetFloat(CLdStringA Path, float def, int index)
	{
		JsonBox::Value val = GetConfigObject(Path, index);
		return val.tryGetFloat(def);
	}

	int CLdConfig::GetInteger(CLdStringA Path, int def, int index)
	{
		JsonBox::Value val = GetConfigObject(Path, index);
		return val.tryGetInteger(def);
	}

	CLdString CLdConfig::GetString(CLdStringA Path, TCHAR* def, int index)
	{
		JsonBox::Value val = GetConfigObject(Path, index);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty())
			result = def;
		return result;
	}
	void CLdConfig::AddArrayValue(CLdStringA Path, JsonBox::Value value)
	{
		int k = GetArrayCount(Path);
		AddConfigObject(Path, value, k);
	}

	JsonBox::Value CLdConfig::GetConfigObject(CLdStringA string, int index)
	{
		int len = string.GetLength();
		char* p = string.GetData();
		JsonBox::Value parent = m_Config;
		JsonBox::Value value = JsonBox::Value::NULL_VALUE;

		for (int i = 0; i < len; i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
			{
				string.GetData()[i] = '\0';
				value = parent[p];
				if (value.isNull())
					break;
				parent = value;
				p = string.GetData() + i + 1;
			}
		}
		value = parent[p];
		if (index != -1)
		{ //数组下标
			if (value.isArray())
				value = value[index];
			else
				value = JsonBox::Value::NULL_VALUE;
		}

		return value;
	}

	void CLdConfig::AddConfigObject(CLdStringA string, JsonBox::Value value, int index)
	{
		int len = string.GetLength();
		CLdMap<char*, JsonBox::Value> objs;
		char* p = string.GetData();
		JsonBox::Value parent = m_Config;

		for (int i = 0; i < len; i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
			{
				string.GetData()[i] = '\0';
				value = parent[p];
				//需要添加不存在的对象,把路径保存下来.以便后面依次向上赋值
				objs.Put(p, parent);

				parent = value;
				p = string.GetData() + i + 1;
			}
		}
		objs.Put(p, parent);
		JsonBox::Value v = parent[p];

		if (index != -1)
		{ //数组下标
			v[index] = value;
		}
		else
			v = value;

		JsonBox::Value *pItem = nullptr;
		for (int i = objs.GetCount() - 1; i >= 0; i--)
		{
			char** pkey = objs.GetAt(i, &pItem);
			(*pItem)[*pkey] = v;
			v = *pItem;
		}
		if (pItem)
			m_Config = *pItem;
	}

	CLdStringW CLdConfig::ToString()
	{
		std::ostringstream stream;
		m_Config.writeToStream(stream, false, false);
		return stream.str();
	}
}

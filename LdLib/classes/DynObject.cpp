#include "stdafx.h"
#include "DynObject.h"
#include <Shlwapi.h>
#include <sstream>
#include "../file/FileUtils.h"

#pragma warning(disable:4018)
#pragma comment(lib,"Shlwapi.lib")

using namespace std;

namespace LeadowLib {

	CDynObject::CDynObject()
		:m_Config()
	{

	}


	CDynObject::~CDynObject()
	{
	}

	BOOL CDynObject::LoadFromFile(TCHAR* FileName)
	{
		try {
			if(!PathFileExists(FileName))
			{
				CLdString tmp = FileName;
				CLdString path((UINT)MAX_PATH);
				CFileUtils::ExtractFilePath(tmp, path.GetData());
				if (CFileUtils::ForceDirectories(path) != 0)
					return FALSE;
			}
			CLdStringA string = FileName;
			m_Config.loadFromFile(string);
			return TRUE;
		}catch(...)
		{
			return FALSE;
		}
	}

	BOOL CDynObject::SaveToFile(TCHAR* FileName)
	{
		try {
			CLdStringA string = FileName;
			m_Config.writeToFile(string);
			return TRUE;
		}
		catch (...)
		{
			return FALSE;
		}
	}

	BOOL CDynObject::PrepareStr(TCHAR* szJson)
	{
		CLdStringA s = szJson;

		m_Config.loadFromString(s.GetData());
		return TRUE;
	}

	void CDynObject::operator=(CDynObject & source)
	{
		m_Config = source.m_Config;
	}

	int CDynObject::GetDataType(CLdStringA Path)
	{
		CDynObjectValue val = GetConfigObject(Path);
		return val.getType();
	}

	int CDynObject::GetArrayCount(CLdStringA Path)
	{
		CDynObjectValue val = GetConfigObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return 0;
		else
		{
			return (int)val.getArray().size();
		}
	}

	BOOL CDynObject::GetBoolean(CLdStringA Path, BOOL def, int index)
	{
		CDynObjectValue val = GetConfigObject(Path, index);
		return val.tryGetBoolean(def == TRUE);
	}

	double CDynObject::GetDouble(CLdStringA Path, double def, int index)
	{
		CDynObjectValue val = GetConfigObject(Path, index);
		return val.tryGetDouble(def);
	}

	float CDynObject::GetFloat(CLdStringA Path, float def, int index)
	{
		CDynObjectValue val = GetConfigObject(Path, index);
		return val.tryGetFloat(def);
	}

	int CDynObject::GetInteger(CLdStringA Path, int def, int index)
	{
		CDynObjectValue val = GetConfigObject(Path, index);
		return val.tryGetInteger(def);
	}

	CLdString CDynObject::GetString(CLdStringA Path, TCHAR* def, int index)
	{
		CDynObjectValue val = GetConfigObject(Path, index);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty() && !def)
			result = def;
		return result;
	}
	void CDynObject::AddArrayValue(CLdStringA Path, CDynObjectValue value)
	{
		int k = GetArrayCount(Path);
		AddConfigObject(Path, value, k);
	}

	CDynObjectValue CDynObject::GetConfigObject(CLdStringA string, int index)
	{
		int len = string.GetLength();
		char* p = string.GetData();
		CDynObjectValue * parent = &m_Config;
		CDynObjectValue * value = nullptr; // JsonBox::Value::NULL_VALUE;

		for (int i = 0; i < len; i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
			{
				string.GetData()[i] = '\0';
				value = &(*parent)[p];
				if (value->isNull())
					break;
				parent = value;
				p = string.GetData() + i + 1;
			}
		}
		value = &(*parent)[p];
		if (index != -1)
		{ //数组下标
			if (value->isArray())
				value = &(*value)[index];
			else
				value = nullptr;// JsonBox::Value::NULL_VALUE;
		}
		if (!value)
			return JsonBox::Value::NULL_VALUE;
		else
			return *value;
	}

	void CDynObject::AddConfigObject(CLdStringA string, CDynObjectValue value, int index)
	{
		int len = string.GetLength();
		//CLdMap<char*, JsonBox::Value> objs;
		char* p = string.GetData();
		CDynObjectValue* parent = &m_Config;

		for (int i = 0; i < len; i++)
		{
			if (string.GetData()[i] == '\\' || string.GetData()[i] == '/')
			{
				string.GetData()[i] = '\0';
				
				if((*parent)[p].isNull())
					(*parent)[p] = JsonBox::Object();
				parent = &(*parent)[p];
				//需要添加不存在的对象,把路径保存下来.以便后面依次向上赋值
				//objs.Put(p, parent);

				//parent = value;
				p = string.GetData() + i + 1;
			}
		}
		//objs.Put(p, parent);
		CDynObjectValue* v = &(*parent)[p];

		if (index != -1)
		{ //数组下标
			(*v)[index] = value;
		}
		else
			*v = value;

//		JsonBox::Value *pItem = nullptr;
//		for (int i = objs.GetCount() - 1; i >= 0; i--)
//		{
//			char** pkey = objs.GetAt(i, &pItem);
//			(*pItem)[*pkey] = v;
//			v = *pItem;
//		}
//		if (pItem)
//			m_Config = *pItem;
	}

	CLdStringW CDynObject::ToString()
	{
		std::ostringstream stream;
		m_Config.writeToStream(stream, false, false);
		std::string s = stream.str();
		return (char*)s.c_str();
	}
}

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
			if(PathFileExists(FileName))
			{
				CLdStringA string = FileName;
				m_Config.loadFromFile(string);
				return TRUE;
			}
			else
				return FALSE;
		}catch(...)
		{
			return FALSE;
		}
	}

	BOOL CDynObject::SaveToFile(TCHAR* FileName)
	{
		try {
			if (!PathFileExists(FileName))
			{
				CLdString path;
				CFileUtils::ExtractFilePath(FileName, path);
				if (CFileUtils::ForceDirectories(path) != 0)
					return FALSE;
			}

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
		CDynObjectValue val = GetDynObject(Path);
		return val.getType();
	}

	int CDynObject::GetArrayCount(CLdStringA Path)
	{
		CDynObjectValue val = GetDynObject(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return 0;
		else
		{
			return (int)val.getArray().size();
		}
	}

	BOOL CDynObject::GetBoolean(CLdStringA Path, BOOL def, int index)
	{
		CDynObjectValue val = GetDynObject(Path, index);
		return val.tryGetBoolean(def == TRUE);
	}

	double CDynObject::GetDouble(CLdStringA Path, double def, int index)
	{
		CDynObjectValue val = GetDynObject(Path, index);
		return val.tryGetDouble(def);
	}

	float CDynObject::GetFloat(CLdStringA Path, float def, int index)
	{
		CDynObjectValue val = GetDynObject(Path, index);
		return val.tryGetFloat(def);
	}

	int CDynObject::GetInteger(CLdStringA Path, int def, int index)
	{
		CDynObjectValue val = GetDynObject(Path, index);
		return val.tryGetInteger(def);
	}

	CLdString CDynObject::GetString(CLdStringA Path, TCHAR* def, int index)
	{
		CDynObjectValue val = GetDynObject(Path, index);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty() && !def)
			result = def;
		return result;
	}
	void CDynObject::AddArrayValue(CLdStringA Path, CDynObjectValue value)
	{
		int k = GetArrayCount(Path);
		AddObjectAttribute(Path, value, k);
	}

	void CDynObject::AddArrayValue(CLdStringA Path, wchar_t* value)
	{
		CLdStringA s = value;
		AddArrayValue(Path, s.GetData());
	}

	CDynObjectValue CDynObject::GetDynObject(CLdStringA string, int index)
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

	void CDynObject::AddObjectAttribute(CLdStringA path, CDynObjectValue value, int index)
	{
		int len = path.GetLength();
		//CLdMap<char*, JsonBox::Value> objs;
		char* p = path.GetData();
		CDynObjectValue* parent = &m_Config;

		for (int i = 0; i < len; i++)
		{
			if (path.GetData()[i] == '\\' || path.GetData()[i] == '/')
			{
				path.GetData()[i] = '\0';
				
				if((*parent)[p].isNull())
					(*parent)[p] = JsonBox::Object();
				parent = &(*parent)[p];
				//需要添加不存在的对象,把路径保存下来.以便后面依次向上赋值
				//objs.Put(p, parent);

				//parent = value;
				p = path.GetData() + i + 1;
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

	void CDynObject::AddObjectAttribute(CLdStringA path, wchar_t* value, int index)
	{
		CLdStringA s = value;
		AddObjectAttribute(path, s.GetData(), index);
	}

	CLdStringW CDynObject::ToString()
	{
		std::ostringstream stream;
		m_Config.writeToStream(stream, false, false);
		std::string s = stream.str();
		return (char*)s.c_str();
	}
}

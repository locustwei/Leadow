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
		:m_Json()
	{

	}

	CDynObject::CDynObject(CDynObject & obj)
	{
		m_Json = obj.m_Json;
	}

	CDynObject::CDynObject(CDynObjectValue json)
	{
		m_Json = json;
	}

	CDynObject::CDynObject(const TCHAR* szJson)
	{
		PrepareStr(szJson);
	}

	CDynObject::~CDynObject()
	{
	}

	BOOL CDynObject::LoadFromFile(const TCHAR* FileName)
	{
		try {
			if(PathFileExists(FileName))
			{
				CLdStringA string = (TCHAR*)FileName;
				m_Json.loadFromFile(string);
				return TRUE;
			}
			else
				return FALSE;
		}catch(...)
		{
			return FALSE;
		}
	}

	BOOL CDynObject::SaveToFile(const TCHAR* FileName)
	{
		try {
			if (!PathFileExists(FileName))
			{
				CLdString path;
				CFileUtils::ExtractFilePath((TCHAR*)FileName, path);
				if (CFileUtils::ForceDirectories(path) != 0)
					return FALSE;
			}

			CLdStringA string = FileName;
			m_Json.writeToFile(string);
			return TRUE;
		}
		catch (...)
		{
			return FALSE;
		}
	}

	BOOL CDynObject::PrepareStr(const TCHAR* szJson)
	{
		CLdStringA s = szJson;

		m_Json.loadFromString(s.GetData());
		return TRUE;
	}

	void CDynObject::operator=(CDynObject & source)
	{
		m_Json = source.m_Json;
	}

	int CDynObject::GetDataType(const TCHAR* Path)
	{
		CDynObjectValue val = GetDynObjectValue(Path);
		return val.getType();
	}

	int CDynObject::GetArrayCount(const TCHAR* Path)
	{
		CDynObjectValue val = GetDynObjectValue(Path);
		if (val.getType() != JsonBox::Value::ARRAY)
			return 0;
		else
		{
			return (int)val.getArray().size();
		}
	}

	BOOL CDynObject::GetBoolean(const TCHAR* Path, BOOL def, int index)
	{
		CDynObjectValue val = GetDynObjectValue((char*)Path, index);
		return val.tryGetBoolean(def);
	}

	double CDynObject::GetDouble(const TCHAR* Path, double def, int index)
	{
		CDynObjectValue val = GetDynObjectValue(Path, index);
		return val.tryGetDouble(def);
	}

	float CDynObject::GetFloat(const TCHAR* Path, float def, int index)
	{
		CDynObjectValue val = GetDynObjectValue(Path, index);
		return val.tryGetFloat(def);
	}

	int64_t CDynObject::GetInteger(const TCHAR* Path, int def, int index)
	{
		CDynObjectValue val = GetDynObjectValue(Path, index);
		return val.tryGetInteger(def);
	}

	CLdString CDynObject::GetString(const TCHAR* Path, TCHAR* def, int index)
	{
		CDynObjectValue val = GetDynObjectValue(Path, index);
		CLdString result;
		result = val.getString().c_str();
		if (result.IsEmpty() && !def)
			result = def;
		return result;
	}

	CDynObject CDynObject::GetDynObject(const TCHAR* Path, int index /*= -1*/)
	{
		return GetDynObjectValue(Path, index);
	}

	void CDynObject::AddArrayValue(const TCHAR* Path, CDynObjectValue value)
	{
		int k = GetArrayCount(Path);
		AddObjectAttribute(Path, value, k);
	}

	void CDynObject::AddArrayValue(const TCHAR* Path, wchar_t* value)
	{
		CLdStringA s = value;
		AddArrayValue(Path, s.GetData());
	}

	VOID CDynObject::AddArrayValue(const TCHAR* Path, CDynObject& value)
	{
		return AddArrayValue(Path, value.m_Json);
	}

	CDynObjectValue CDynObject::GetDynObjectValue(CLdStringA string, int index)
	{
		int len = string.GetLength();
		char* p = string;

		CDynObjectValue * parent = &m_Json;
		CDynObjectValue * value = nullptr; // JsonBox::Value::NULL_VALUE;

		for (int i = 0; i < len; i++)
		{
			if (string[i] == '\\' || string[i] == '/')
			{
				string.GetData()[i] = '\0';
				value = &(*parent)[p];
				if (value->isNull())
					return JsonBox::Value();
				parent = value;
				p = string + i + 1;
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
			return JsonBox::Value();
		else
			return *value;
	}

	void CDynObject::AddObjectAttribute(const TCHAR* Path, CDynObjectValue value, int index)
	{
		CLdStringA path = Path;
		int len = path.GetLength();
		//CLdMap<char*, JsonBox::Value> objs;
		char* p = path.GetData();
		CDynObjectValue* parent = &m_Json;

		for (int i = 0; i < len; i++)
		{
			if (path.GetData()[i] == '\\' || path.GetData()[i] == '/')
			{
				path.GetData()[i] = '\0';
				
				if((*parent)[p].isNull())
					(*parent)[p] = JsonBox::Object();
				parent = &(*parent)[p];
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

	}

	void CDynObject::AddObjectAttribute(const TCHAR* path, wchar_t* value, int index)
	{
		CLdStringA s = value;
		AddObjectAttribute(path, s.GetData(), index);
	}

	void CDynObject::AddObjectAttribute(const TCHAR* path, CDynObject& obj, int index)
	{
		AddObjectAttribute(path, obj.m_Json, index);
	}

	CLdString CDynObject::ToString()
	{
		std::ostringstream stream;
		m_Json.writeToStream(stream, false, false);
		std::string s = stream.str();
		return (char*)s.c_str();
	}
}

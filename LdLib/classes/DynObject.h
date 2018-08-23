#pragma once

/*
基于JSON实现的“动态对象”，用于一些不确定格式的数据同一管理。
如：配置信息，命令行参数。
*/

#include "LdString.h"
#include "../../Jsonlib/Value.h"
namespace LeadowLib {

	#define  CDynObjectValue JsonBox::Value

	class CDynObject
	{
	public:
		CDynObject();
		CDynObject(const TCHAR* szJson);
		~CDynObject();

		BOOL LoadFromFile(const TCHAR* FileName);
		BOOL SaveToFile(const TCHAR* FileName);
		BOOL PrepareStr(const TCHAR* szJson);
		void operator = (CDynObject& source);   

		int GetDataType(const TCHAR* Path); //类型参见JsonBox::Value::Type
		int GetArrayCount(const TCHAR* Path);
		BOOL GetBoolean(const TCHAR* Path, BOOL def = FALSE, int index = -1);
		double GetDouble(const TCHAR* Path, double def = 0.0, int index = -1);
		float GetFloat(const TCHAR* Path, float def = 0.0, int index = -1);
		int64_t GetInteger(const TCHAR* Path, int def = 0, int index = -1);
		CLdString GetString(const TCHAR* Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //添加数组项目
			const TCHAR* Path,
			CDynObjectValue value
		);

		VOID AddArrayValue(                                         //添加数组项目
			const TCHAR* Path,
			CDynObject& value
		);

		VOID AddArrayValue(                                         //添加数组项目
			const TCHAR* Path,
			wchar_t* value
		);

		void AddObjectAttribute(                                     //添加属性值
			const TCHAR*,                                           //路径
			CDynObjectValue value,                                   //值
			int index = -1                                          //数组下标：-1不是数组。
		);

		void AddObjectAttribute(                                       //添加属性值
			const TCHAR* path,                                      //路径
			wchar_t* value,                                       //值
			int index = -1                                          //数组下标：-1不是数组。
		);

		void AddObjectAttribute(                                       //添加属性值
			const TCHAR* path,                                      //路径
			CDynObject& obj,                                       //值
			int index = -1                                          //数组下标：-1不是数组。
		);

		CLdString ToString();
	protected:
		CDynObjectValue m_Json;
//		CLdStringA m_ConfigFileName;

	private:
		CDynObjectValue GetDynObject(                             //获取指定路径下的对象
			CLdStringA string,                                      //路径
			int index = -1                                          //如果对象是数组，这是数组下标
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //数据类如果不为null_value,则当对象不存在时创建一个
//			PVOID pValue=nullptr                                    //如果创建,对象值
		);
	};
}

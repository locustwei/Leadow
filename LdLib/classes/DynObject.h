#pragma once

/*
配置文件读写基类（JSON格式），各功能模块配置文件从此继承
*/

#include "LdString.h"
#include "../../Jsonlib/Value.h"
namespace LeadowLib {

	#define  CDynObjectValue JsonBox::Value

	class CDynObject
	{
	public:
		CDynObject();
		~CDynObject();

		BOOL LoadFromFile(TCHAR* FileName);
		BOOL SaveToFile(TCHAR* FileName);
		BOOL PrepareStr(TCHAR* szJson);
		void operator = (CDynObject& source);   

		int GetDataType(CLdStringA Path); //类型参见JsonBox::Value::Type
		int GetArrayCount(CLdStringA Path);
		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE, int index = -1);
		double GetDouble(CLdStringA Path, double def = 0.0, int index = -1);
		float GetFloat(CLdStringA Path, float def = 0.0, int index = -1);
		int GetInteger(CLdStringA Path, int def = 0, int index = -1);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //添加数组项目
			CLdStringA Path, 
			CDynObjectValue value
		);
		void AddConfigObject(                                       //添加属性值
			CLdStringA string,                                      //路径
			CDynObjectValue value,                                   //值
			int index = -1                                          //数组下标：-1不是数组。
		);
		CLdString ToString();
	protected:
		CDynObjectValue m_Config;
//		CLdStringA m_ConfigFileName;

	private:
		CDynObjectValue GetConfigObject(                             //获取指定路径下的对象
			CLdStringA string,                                      //路径
			int index = -1                                          //如果对象是数组，这是数组下标
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //数据类如果不为null_value,则当对象不存在时创建一个
//			PVOID pValue=nullptr                                    //如果创建,对象值
		);
	};
}

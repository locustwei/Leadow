#pragma once

/*
配置文件读写基类（JSON格式），各功能模块配置文件从此继承
*/

#include "../Jsonlib/JsonBox.h"

namespace LeadowLib {
	class CLdConfig
	{
	public:
		CLdConfig();
		~CLdConfig();

		BOOL LoadConfig();        //加载配置文件
		BOOL SaveConfig();        //写入配置文件
		BOOL PrepareStr(TCHAR* szJson);
		void operator = (CLdConfig& source);    //Copy配置文件（内容不包含文件路径）

		int GetDataType(CLdStringA Path); //类型参见JsonBox::Value::Type
		int GetArrayCount(CLdStringA Path);
		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE, int index = -1);
		double GetDouble(CLdStringA Path, double def = 0.0, int index = -1);
		float GetFloat(CLdStringA Path, float def = 0.0, int index = -1);
		int GetInteger(CLdStringA Path, int def = 0, int index = -1);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr, int index = -1);

		VOID AddArrayValue(                                         //添加数组项目
			CLdStringA Path, 
			JsonBox::Value value
		);
		void AddConfigObject(                                       //添加属性值
			CLdStringA string,                                      //路径
			JsonBox::Value value,                                   //值
			int index = -1                                          //数组下标：-1不是数组。
		);
		CLdString ToString();
	protected:
		JsonBox::Value m_Config;
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value GetConfigObject(                             //获取指定路径下的对象
			CLdStringA string,                                      //路径
			int index = -1                                          //如果对象是数组，这是数组下标
//			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //数据类如果不为null_value,则当对象不存在时创建一个
//			PVOID pValue=nullptr                                    //如果创建,对象值
		);
	};
}

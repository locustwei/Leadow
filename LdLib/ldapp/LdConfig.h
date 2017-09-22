#pragma once

/*
配置文件读写基类（JSON格式），各功能模块配置文件从此继承
*/

#include "../../Jsonlib/JsonBox.h"
#include "../classes/LdString.h"

namespace LeadowLib {
	class CLdConfig
	{
	public:
		CLdConfig();
		~CLdConfig();

		BOOL LoadConfig();        //加载配置文件
		BOOL SaveConfig();        //写入配置文件

		void operator = (CLdConfig& source);    //Copy配置文件（内容不包含文件路径）

		int GetDataType(CLdStringA Path);

		int GetArrayCount(CLdStringA Path);
		BOOL GetArrayBoolean(CLdStringA Path, int index, BOOL def = FALSE);
		double GetArrayDouble(CLdStringA Path, int index, double def=0.0);
		float GetArrayFloat(CLdStringA Path, int index, float def=0.0);
		int GetArrayInteger(CLdStringA Path, int index, int def=0);
		CLdString GetArrayString(CLdStringA Path, int index, TCHAR* def=nullptr);

		VOID SetArrayBoolean(CLdStringA Path, int index, BOOL Value);
		VOID SetArrayDouble(CLdStringA Path, int index, double Value);
		VOID SetArrayFloat(CLdStringA Path, int index, float Value);
		VOID SetArrayInteger(CLdStringA Path, int index, int Value);
		VOID SetArrayString(CLdStringA Path, int index, TCHAR* Value);


		BOOL GetBoolean(CLdStringA Path, BOOL def = FALSE);
		double GetDouble(CLdStringA Path, double def = 0.0);
		float GetFloat(CLdStringA Path, float def = 0.0);
		int GetInteger(CLdStringA Path, int def = 0);
		CLdString GetString(CLdStringA Path, TCHAR* def = nullptr);

		VOID SetBoolean(CLdStringA Path, BOOL Value);
		VOID SetDouble(CLdStringA Path, double Value);
		VOID SetFloat(CLdStringA Path, float Value);
		VOID SetInteger(CLdStringA Path, int Value);
		VOID SetString(CLdStringA Path, TCHAR* Value);
	protected:
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value m_Config;
		JsonBox::Value GetConfigObject(                             //获取指定路径下的对象
			CLdStringA string,                                      //路径
			int index = 0,                                          //如果对象是数组，这是数组下标
			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,  //数据类如果不为null_value,则当对象不存在时创建一个
			PVOID pValue=nullptr                                    //如果创建,对象值
		);
	};
}

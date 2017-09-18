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

		int GetDataType(TCHAR* Path);

		//	int GetArrayCount(TCHAR* Path);
		//	BOOL GetArrayBoolean(TCHAR* Path, int index);
		//	double GetArrayDouble(TCHAR* Path, int index);
		//	float GetArrayFloat(TCHAR* Path, int index);
		//	int GetArrayInteger(TCHAR* Path, int index);
		//	CLdString GetArrayString(TCHAR* Path, int index);

		BOOL GetBoolean(TCHAR* Path);       //Path格式 "****\*****"或"*****/****"
		double GetDouble(TCHAR* Path);
		float GetFloat(TCHAR* Path);
		int GetInteger(TCHAR* Path);
		CLdString GetString(TCHAR* Path);

		BOOL GetBoolean(char* Path, BOOL def = FALSE);
		double GetDouble(char* Path, double def = 0.0);
		float GetFloat(char* Path, float def = 0.0);
		int GetInteger(char* Path, int def = 0);
		CLdString GetString(char* Path, TCHAR* def = nullptr);

		VOID SetBoolean(char* Path, BOOL Value);
		VOID SetDouble(char* Path, double Value);
		VOID SetFloat(char* Path, float Value);
		VOID SetInteger(char* Path, int Value);
		VOID SetString(char* Path, TCHAR* Value);
	protected:
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value m_Config;
		JsonBox::Value GetConfigObject(    //获取指定路径下的对象
			CLdStringA string,             //路径
			JsonBox::Value::Type type= JsonBox::Value::NULL_VALUE,       //数据类如果不为null_value,则当对象不存在时创建一个
			PVOID pValue=nullptr                                         //如果创建,对象值
		);
	};
}

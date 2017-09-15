#pragma once
#include "../../Jsonlib/JsonBox.h"
#include "../classes/LdString.h"

namespace LeadowLib {
	class CLdConfig
	{
	public:
		CLdConfig();
		~CLdConfig();

		BOOL LoadConfig();
		BOOL SaveConfig();

		int GetDataType(TCHAR* Path);

		//	int GetArrayCount(TCHAR* Path);
		//	BOOL GetArrayBoolean(TCHAR* Path, int index);
		//	double GetArrayDouble(TCHAR* Path, int index);
		//	float GetArrayFloat(TCHAR* Path, int index);
		//	int GetArrayInteger(TCHAR* Path, int index);
		//	CLdString GetArrayString(TCHAR* Path, int index);

		BOOL GetBoolean(TCHAR* Path);
		double GetDouble(TCHAR* Path);
		float GetFloat(TCHAR* Path);
		int GetInteger(TCHAR* Path);
		CLdString GetString(TCHAR* Path);

		BOOL GetBoolean(char* Path, BOOL def);
		double GetDouble(char* Path, double def);
		float GetFloat(char* Path, float def);
		int GetInteger(char* Path, int def);
		CLdString GetString(char* Path, TCHAR* def);

		VOID SetBoolean(TCHAR* Path, BOOL Value);
		VOID SetDouble(TCHAR* Path, double Value);
		VOID SetFloat(TCHAR* Path, float Value);
		VOID SetInteger(TCHAR* Path, int Value);
		VOID SetString(TCHAR* Path, TCHAR* Value);
	protected:
		CLdStringA m_ConfigFileName;
	private:
		JsonBox::Value m_Config;
		JsonBox::Value GetConfigObject(TCHAR* Path);
	};
}

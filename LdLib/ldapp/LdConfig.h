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
		JsonBox::Value GetConfigObject(CLdStringA string);
	};
}

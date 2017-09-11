#pragma once

class CErasureConfig
{
public:
	CErasureConfig();
	~CErasureConfig();

	BOOL LoadConfig();
	BOOL SaveConfig();

	int GetDataType(TCHAR* Path);

	int GetArrayCount(TCHAR* Path);
	BOOL GetArrayBoolean(TCHAR* Path, int index);
	double GetDouble(TCHAR* Path, int index);
	float GetFloat(TCHAR* Path, int index);
	int GetInteger(TCHAR* Path, int index);
	CLdString GetString(TCHAR* Path, int index);

	BOOL GetBoolean(TCHAR* Path);
	double GetDouble(TCHAR* Path);
	float GetFloat(TCHAR* Path);
	int GetInteger(TCHAR* Path);
	CLdString GetString(TCHAR* Path);
};

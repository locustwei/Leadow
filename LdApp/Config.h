#pragma once

/*
配置文件管理单元（读取、写入JSON文件格式），配置文件路径在%appdata%下
*/


class CConfig
	:public CDynObject
{
public:
	CConfig();
	~CConfig();
	//Eraser-----------------------------------------------------------
	int GetFileErasureMothed();
	BOOL IsRemoveFolder();	
	int GetVolumeErasureMethed();
	BOOL IsSkipSpace();
	BOOL IsSkipTrack();
	BOOL IsErasureFreeFileSpace();
	BOOL IsShutDown();
	void SetFileErasureMothed(int Mothed);
	void SetRemoveFolder(BOOL value);
	void SetVolumeErasureMethed(int Mothed);
	void SetSkipSpace(BOOL value);
	void SetSkipTrack(BOOL value);
	void SetErasureFreeFileSpace(BOOL value);
	void SetShutDown(BOOL value);
	//--------------------------------------------------------------
	//static void LoadConfig();
private:
	
};

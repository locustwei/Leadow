#pragma once

/*
配置文件管理单元（读取、写入JSON文件格式），配置文件路径在%appdata%下
*/

#include "ErasureMethod.h"

class CErasureConfig
	:public CLdConfig
{
public:
	CErasureConfig();
	~CErasureConfig();

	CErasureMethod& GetFileErasureMothed();
	int GetFileErasureMothedIndex();
	BOOL IsRemoveFolder();
	
	CErasureMethod& GetVolumeErasureMethed();
	int GetVolumeErasureMethedIndex();
	BOOL IsSkipSpace();
	BOOL IsSkipTrack();
	BOOL IsErasureFreeFileSpace();
	BOOL IsShutDown();

	void SetFileErasureMothed(int MothedIndex);
	void SetRemoveFolder(BOOL value);

	void SetVolumeErasureMethed(int MothedIndex);
	void SetSkipSpace(BOOL value);
	void SetSkipTrack(BOOL value);
	void SetErasureFreeFileSpace(BOOL value);
	void SetShutDown(BOOL value);

private:
	
};

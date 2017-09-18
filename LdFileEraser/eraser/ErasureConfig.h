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

	//ErasureMothedType GetFileErasureMothed();
	ErasureMothedType GetFileErasureMothed();
	BOOL IsRemoveFolder();
	
	//CErasureMothed& GetVolumeErasureMethed();
	ErasureMothedType GetVolumeErasureMethed();
	BOOL IsSkipSpace();
	BOOL IsSkipTrack();
	BOOL IsErasureFreeFileSpace();
	BOOL IsShutDown();

	void SetFileErasureMothed(ErasureMothedType Mothed);
	void SetRemoveFolder(BOOL value);

	void SetVolumeErasureMethed(ErasureMothedType Mothed);
	void SetSkipSpace(BOOL value);
	void SetSkipTrack(BOOL value);
	void SetErasureFreeFileSpace(BOOL value);
	void SetShutDown(BOOL value);

private:
	
};

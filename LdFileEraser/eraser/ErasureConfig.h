#pragma once

/*
�����ļ�����Ԫ����ȡ��д��JSON�ļ���ʽ���������ļ�·����%appdata%��
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

#pragma once

/*
��Ʒ�����ļ�����Ԫ����ȡ��д��JSON�ļ���ʽ���������ļ�·����%appdata%�¡�
CAppConfig ��Ʒ�����ļ�
CErasureConfig �ļ����������ļ�
*/

class CErasureConfig
	:public CDynObject
{
public:
	CErasureConfig();
	~CErasureConfig();

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
};

extern CErasureConfig ErasureConfig;
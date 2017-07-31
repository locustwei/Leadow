#pragma once

#include "stdafx.h"
#include "DiskVolume.h"

class CHardisk
{
public:
	CHardisk(void);
	~CHardisk(void);

	static CHardisk *CreateHardisk(ULONG nIndex);
private:
	HANDLE m_hDisk;
	std::CLdArray<CDiskVolume*> m_Volumes;
	VOID GetDiskPartitions();
};


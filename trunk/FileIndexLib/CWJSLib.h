#pragma once

#include "stdafx.h"
#include "DiskVolume.h"
#include "ExportFunction.h"

class CWJSLib: public IWJLibInterface
{

public:
	CWJSLib(void);
	~CWJSLib(void);

	virtual BOOL  SetDumpFilePath(PCWSTR wsz_path) override;
	virtual UINT  GetVolumeCount() override;
	virtual IVolumeInterface*  GetVolume(DWORD idx) override;
private:

	CLdArray<CDiskVolume*> m_Volumes;
	VOID EnumDiskVolumes();


};


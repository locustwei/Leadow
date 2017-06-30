#pragma once

#include "stdafx.h"
#include "DiskVolume.h"
#include "ExportFunction.h"

class CWJSLib: public IWJLibInterface
{

public:
	CWJSLib(void);
	~CWJSLib(void);

	virtual BOOL __stdcall SetDumpFilePath(PCWSTR wsz_path) override;
	virtual UINT __stdcall GetVolumeCount() override;
	virtual IVolumeInterface* __stdcall GetVolume(ULONG idx) override;
private:

	vector<CDiskVolume*> m_Volumes;
	VOID EnumDiskVolumes();


};


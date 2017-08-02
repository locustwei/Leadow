// LdFileSearch.h : LdFileSearch DLL 的主头文件
//

#pragma once
#include "MftReader/MftReader.h"

class LDLIB_API ISearchLibrary
{
public:
	virtual ~ISearchLibrary() {};

	virtual UINT64 EnumVolumeFiles(CVolumeInfo* pVolume, IMftReadeHolder* callback, PVOID Context) = 0;
private:
};

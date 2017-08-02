// LdFileSearch.h : LdFileSearch DLL ����ͷ�ļ�
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

// LdFileSearch.h : LdFileSearch DLL 的主头文件
//

#pragma once
#include "../MftLib/FatMftReader.h"

class LDLIB_API ISearchLibrary
{
public:
	virtual ~ISearchLibrary() {};
	/*
	读取磁盘文件分配表中的文件记录。
	PVOID Context （有一个特殊值Fat时传0xFF1调用擦除删除文件记录）
	*/
	virtual UINT64 EnumVolumeFiles(CVolumeInfo* pVolume, IMftReadeHolder* callback, UINT_PTR Context) = 0;
private:
};

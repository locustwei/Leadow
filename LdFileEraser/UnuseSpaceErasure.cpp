#include "stdafx.h"
#include "UnuseSpaceErasure.h"


CUnuseSpaceErasure::CUnuseSpaceErasure()
{
}


CUnuseSpaceErasure::~CUnuseSpaceErasure()
{
}

DWORD CUnuseSpaceErasure::Execute(CLdString & Driver, CErasureMethod& method, IErasureCallback* callback)
{
	//检查是否有管理员权限（错误）
	//检查是否有系统还原点（警告）
	//检查磁盘用户配额（警告 GetDiskFreeSpaceEx）

	CVolumeInfo volInfo;
	volInfo.OpenVolumePath(Driver);
	DWORD dwAttribute = GetFileAttributes(Driver);  //FILE_ATTRIBUTE_REPARSE_POINT
	if (CFileUtils::IsCompressed(Driver))
		CFileUtils::SetCompression(Driver, FALSE);

	return 0;
}

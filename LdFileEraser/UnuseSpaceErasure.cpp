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
	//����Ƿ��й���ԱȨ�ޣ�����
	//����Ƿ���ϵͳ��ԭ�㣨���棩
	//�������û������� GetDiskFreeSpaceEx��

	CVolumeInfo volInfo;
	volInfo.OpenVolumePath(Driver);
	DWORD dwAttribute = GetFileAttributes(Driver);  //FILE_ATTRIBUTE_REPARSE_POINT
	return 0;
}

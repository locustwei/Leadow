#include "stdafx.h"
#include "UnuseSpaceErasure.h"

#define Erasure_temp_path _T("___Leadow_Erasure_tmp__")

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
	DWORD Result = 0;

	do
	{
		if(!callback->ErasureStart(3))
			break;
		m_callback = callback;
		m_method = &method;

		Result = m_volInfo.OpenVolumePath(Driver);
		if (Result != 0)
			break;
		DWORD dwAttribute = GetFileAttributes(Driver);  //FILE_ATTRIBUTE_REPARSE_POINT
		BOOL IsCompressed = CFileUtils::IsCompressed(Driver);
		if (IsCompressed)
			CFileUtils::SetCompression(Driver, FALSE);
		m_tmpDir = Driver;
		m_tmpDir += Erasure_temp_path;
		Result = CFileUtils::ForceDirectories(m_tmpDir);
		if (Result != 0)
			break;


	} while (false);
	
	callback->ErasureCompleted(Result);

	return Result;
}

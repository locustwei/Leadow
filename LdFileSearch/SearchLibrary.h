// LdFileSearch.h : LdFileSearch DLL ����ͷ�ļ�
//

#pragma once
#include "../MftLib/FatMftReader.h"

class LDLIB_API ISearchLibrary
{
public:
	virtual ~ISearchLibrary() {};
	/*
	��ȡ�����ļ�������е��ļ���¼��
	PVOID Context ����һ������ֵFatʱ��0xFF1���ò���ɾ���ļ���¼��
	*/
	virtual UINT64 EnumVolumeFiles(CVolumeInfo* pVolume, IMftReadeHolder* callback, UINT_PTR Context) = 0;
private:
};

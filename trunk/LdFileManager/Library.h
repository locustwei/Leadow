#pragma once

/*
����ģ�鶯̬�����Ԫ��
*/

#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	//�����ļ�������̬��
	static IErasureLibrary* LoadErasureLibrary();
	//�ͷ��ļ�������̬��
	static void FreeErasureLibrary();
private:
	//������̬�����
	static CControlUI * BuildXml(TCHAR * skinXml);
	static PVOID InitLib(TCHAR * pLibFile, CLdApp* ThisApp);
	static void FreeLib(TCHAR* pLibFile);

};

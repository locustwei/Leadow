#pragma once

/*
���ع���ģ�鶯̬��
*/

#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	//�����ļ�������̬��
	static IErasureLibrary* LoadEraserLarary(CPaintManagerUI* pm, CLdApp* ThisApp);
	static IErasureLibrary* LoadProtectLarary(CLdApp* ThisApp);
	static CControlUI * BuildXml(TCHAR * skinXml, CPaintManagerUI* pm);
};

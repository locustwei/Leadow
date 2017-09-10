#pragma once

/*
加载功能模块动态库
*/

#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	//加载文件擦除动态库
	static IErasureLibrary* LoadEraserLarary(CPaintManagerUI* pm, CLdApp* ThisApp);
	static IErasureLibrary* LoadProtectLarary(CLdApp* ThisApp);
	static CControlUI * BuildXml(TCHAR * skinXml, CPaintManagerUI* pm);
};

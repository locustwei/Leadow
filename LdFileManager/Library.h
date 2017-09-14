#pragma once

/*
功能模块动态库管理单元。
*/

#include "../LdFileEraser/ErasureLibrary.h"
#include "../LdFileProtect/ProtectLibrary.h"

class CLdLibray
{
public:
	//加载文件擦除动态库
	static IErasureLibrary* LoadErasureLibrary();
	//释放文件擦除动态库
	static void FreeErasureLibrary();
private:
	//创建动态库界面
	static CControlUI * BuildXml(TCHAR * skinXml);
	static PVOID InitLib(TCHAR * pLibFile, CLdApp* ThisApp);
	static void FreeLib(TCHAR* pLibFile);

};

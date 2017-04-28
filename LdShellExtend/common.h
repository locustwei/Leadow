#pragma once

#define REG_KEYNAME _T("Leadow Shell Extend")       //注册表键名
#define GUID_STR _T("{7C4429E3-0593-45DC-BF37-F2CBCEFB27F2}")    //
#define REG_TYPE_VALUE _T("ExtendType")     //注册表扩展类型标识（判定调用的是那个扩展类型）

//扩展类型枚举
typedef enum SHELL_EXTEND_TYPE
{
	SHELL_EXTEND_TYPE_NONE = 0,
	CONTEX_MENU_ALLFILE = 0x100,  //所有文件右键菜单
	CONTEX_MENU_FOLDER  = 0x101,  //文件夹
	CONTEX_MENU_DIR_BKG = 0x102,  //文件夹空白处
	CONTEX_MENU_DRIVE   = 0x103,  //磁盘
};

void inline OutputDebugStringEx(LPCTSTR strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscwprintf(strOutputString, vlArgs) + 1;
	wchar_t *strBuffer = new wchar_t[nLen];
	_vsnwprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringW(strBuffer);
	delete [] strBuffer;
}

void inline OutputDebugStringEx(LPCSTR strOutputString, ...)
{
	va_list vlArgs = NULL;
	va_start(vlArgs, strOutputString);
	size_t nLen = _vscprintf(strOutputString, vlArgs) + 1;
	char *strBuffer = new char[nLen];
	_vsnprintf_s(strBuffer, nLen, nLen, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugStringA(strBuffer);
	delete [] strBuffer;
}
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

// Windows 头文件:
#include <windows.h>

//#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE
#define PLUGIN_UI_CLASS CFramNotifyPump

#pragma warning(disable:4091) //warning C4091: “typedef ”: 没有声明变量时忽略“tagGPFIDL_FLAGS”的左侧
#pragma warning(disable:4996) //'wcscpy': This function or variable may be unsafe. Consider using wcscpy_s instead. 
#pragma warning(disable:4800) //“BOOL”: 将值强制为布尔值“true”或“false”(性能警告)

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "targetver.h"
#include <LdLib.h>
#include <Uilib.h>
#include <JsonBox.h>
#include <LdApp.h>
#include <LdMft.h>

using namespace DuiLib;
using namespace LeadowLib;
using namespace JsonBox;
using namespace LeadowDisk;

#ifdef _DEBUG
#ifdef WIN64
#pragma comment(lib,"LdLib_d64.lib")
#pragma comment(lib,"DuiLib_d64.lib")
#pragma comment(lib,"DiskLib_d64.lib")
#pragma comment(lib,"JsonLib_d64.lib")
#pragma comment(lib,"LdApp_d64.lib")
#else
#pragma comment(lib,"LdLib_d32.lib")
#pragma comment(lib,"DuiLib_d32.lib")
#pragma comment(lib,"DiskLib_d32.lib")
#pragma comment(lib,"JsonLib_d32.lib")
#pragma comment(lib,"LdApp_d32.lib")
#endif
#else
#ifdef _X64
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"DiskLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"DiskLib.lib")
#endif
#endif
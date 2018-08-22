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

#define COMM_PIPE_NAME _T("8E557ACB-D1D3-4D30-989D-ECA43B8A9BDE")
#define IMPL_PLUGIN_ID _T("BCBE2CB1-37FC-46C2-A9A2-9B9EEBEC262E")

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "targetver.h"
#include <LdLib.h>
#include <Uilib.h>
#include <JsonBox.h>
#include <LdMft.h>
#include <LdApp.h>

using namespace DuiLib;
using namespace LeadowLib;
using namespace JsonBox;

#ifdef _DEBUG
#ifdef WIN64
#pragma comment(lib,"LdLib_d64.lib")
#pragma comment(lib,"DuiLib_d64.lib")
#pragma comment(lib,"MftLib_d64.lib")
#pragma comment(lib,"JsonLib_d64.lib")
#pragma comment(lib,"LdApp_d64.lib")
#else
#pragma comment(lib,"LdLib_d32.lib")
#pragma comment(lib,"DuiLib_d32.lib")
#pragma comment(lib,"MftLib_d32.lib")
#pragma comment(lib,"JsonLib_d32.lib")
#pragma comment(lib,"LdApp_d32.lib")
#endif
#else
#ifdef _X64
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#endif
#endif
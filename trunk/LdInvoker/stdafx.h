// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <windows.h>

#include <LdLib.h>
using namespace LeadowLib;
#include <UIlib.h>
using namespace DuiLib;

#include "Communications.h"

#ifdef _DEBUG
#ifdef WIN64
#pragma comment(lib,"LdLib_d64.lib")
#pragma comment(lib,"jsonlib_d64.lib")
#pragma comment(lib,"DuiLib_d64.lib")
#pragma comment(lib,"Communications_d64.lib")
#else
#pragma comment(lib,"LdLib_d32.lib")
#pragma comment(lib,"jsonlib_d32.lib")
#pragma comment(lib,"DuiLib_d32.lib")
#pragma comment(lib,"Communications_d32.lib")
#endif
#else
#ifdef WIN64
#pragma comment(lib,"LdLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#endif
#endif

// TODO: 在此处引用程序需要的其他头文件

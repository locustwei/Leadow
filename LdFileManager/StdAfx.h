#pragma once

//#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#ifdef _DEBUG
#ifdef WIN64
#pragma comment(lib,"LdLib_d64.lib")
#pragma comment(lib,"DuiLib_d64.lib")
#pragma comment(lib,"MftLib_d64.lib")
#pragma comment(lib,"ReErasureUI_d64.lib")
#pragma comment(lib,"Jsonlib_d64.lib")
#pragma comment(lib,"Communications_d64.lib")
#pragma comment(lib,"LdApp_d64.lib")
#else
#pragma comment(lib,"DuiLib_d32.lib")
#pragma comment(lib,"LdLib_d32.lib")
#pragma comment(lib,"MftLib_d32.lib")
#pragma comment(lib,"ReErasureUI_d32.lib")
#pragma comment(lib,"Jsonlib_d32.lib")
#pragma comment(lib,"Communications_d32.lib")
#pragma comment(lib,"LdApp_d32.lib")
#endif
#else
#ifdef WIN64
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#pragma comment(lib,"ReErasureUI.lib")
#else
#pragma comment(lib,"LdLib.lib")
#pragma comment(lib,"DuiLib.lib")
#pragma comment(lib,"MftLib.lib")
#pragma comment(lib,"ReErasureUI.lib")
#endif
#endif

#pragma warning(disable:4091 4996) //warning C4091: “typedef ”: 没有声明变量时忽略“tagGPFIDL_FLAGS”的左侧

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include "LdLib.h"
#include "UIlib.h"

using namespace DuiLib;
using namespace LeadowLib;

#include "EraserUI.h"


#pragma once

#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE

#ifdef _DEBUG
#ifdef _X64
#pragma comment(lib,"LdLib_d64.lib")
#pragma comment(lib,"DuiLib_d64.lib")
#pragma comment(lib,"MftLib_d64.lib")
#else
#pragma comment(lib,"DuiLib_d32.lib")
#pragma comment(lib,"LdLib_d32.lib")
#pragma comment(lib,"MftLib_d32.lib")
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

#pragma warning(disable:4091) //warning C4091: ��typedef ��: û����������ʱ���ԡ�tagGPFIDL_FLAGS�������

#include <windows.h>
#include <objbase.h>
#include <zmouse.h>
#include "LdLib.h"
#include "UIlib.h"

using namespace DuiLib;
using namespace LeadowLib;


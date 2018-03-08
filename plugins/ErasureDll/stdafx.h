// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

// Windows ͷ�ļ�:
#include <windows.h>

//#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECATE
#define PLUGIN_UI_CLASS CFramNotifyPump

#pragma warning(disable:4091) //warning C4091: ��typedef ��: û����������ʱ���ԡ�tagGPFIDL_FLAGS�������
#pragma warning(disable:4996) //'wcscpy': This function or variable may be unsafe. Consider using wcscpy_s instead. 

// C ����ʱͷ�ļ�
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
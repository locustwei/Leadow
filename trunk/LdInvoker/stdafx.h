// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ

#include <windows.h>

#include <LdLib.h>
using namespace LeadowLib;

#ifdef _DEBUG
#ifdef _X64
#pragma comment(lib,"LdLib_d64.lib")
#else
#pragma comment(lib,"LdLib_d32.lib")
#endif
#else
#ifdef _X64
#pragma comment(lib,"LdLib.lib")
#else
#pragma comment(lib,"LdLib.lib")
#endif
#endif

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

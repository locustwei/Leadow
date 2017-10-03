// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

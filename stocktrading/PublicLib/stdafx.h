// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include "windows.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

#ifdef _WIN64

#define GW_WNDPROC  GWLP_WNDPROC

#else

#define GW_WNDPROC  GWL_WNDPROC

#endif /* _WIN64 */

#define MAX_LEN 255

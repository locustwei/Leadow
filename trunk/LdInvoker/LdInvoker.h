#pragma once

#include "resource.h"

#define ERROR_PARAMCOUNT -1
#define ERROR_LOAD_LIBRARY -2
#define ERROR_FUNCTION_NOT_FOUND -3

#define MAX_LOADSTRING 100

extern HINSTANCE hInst;								// ��ǰʵ��
extern TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
extern TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

extern LPTSTR* ParamStrs;
extern int ParamCount;

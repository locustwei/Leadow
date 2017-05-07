#pragma once

#include "resource.h"

#define ERROR_PARAMCOUNT -1
#define ERROR_LOAD_LIBRARY -2
#define ERROR_FUNCTION_NOT_FOUND -3

#define MAX_LOADSTRING 100

extern HINSTANCE hInst;								// 当前实例
extern TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
extern TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

extern LPTSTR* ParamStrs;
extern int ParamCount;

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#include <Windows.h>
#include "string_x.h"
using namespace std;

#define HIDWORD(a) ((DWORD)((ULONGLONG)(a) >> 32))
#define LODWORD(a) ((DWORD)((ULONGLONG)(a) & 0xFFFFFFFF))

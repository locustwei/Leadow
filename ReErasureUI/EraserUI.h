#pragma once

#include "ErasureFileUI.h"
#include "ErasureMainWnd.h"
#include "ErasureOptions.h"

DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<TCHAR*>* files);
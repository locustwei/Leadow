#pragma once

#include "ErasureFileUI.h"
#include "ErasureMainWnd.h"
#include "ErasureOptions.h"

//�������ھ�̬����ʵ�֣���Ϊ����һ������⡣
DWORD ExecuteFileErase(IEraserListen* callback, CLdArray<TCHAR*>* files);
DWORD ExecuteFileAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files);

DWORD ExecuteVolumeAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files);

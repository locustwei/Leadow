#pragma once

#include "ErasureFileUI.h"
#include "ErasureMainWnd.h"
#include "ErasureOptions.h"

//函数不在静态库中实现，因为这是一个界面库。
//inline DWORD ExecuteFileErase(IEraserListen* callback, CLdArray<TCHAR*>* files)
//{
//	return 0;
//}
//
//inline DWORD ExecuteFileAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files)
//{
//	return 0;
//}
//
//inline DWORD ExecuteVolumeAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files)
//{
//	return 0;
//}

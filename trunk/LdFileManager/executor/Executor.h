#pragma once


/*

*/

DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<TCHAR*>* files);
bool EraserThreadCallback(CLdString fileName, E_THREAD_OPTION op, DWORD dwValue);
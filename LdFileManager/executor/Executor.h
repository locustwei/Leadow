#pragma once


/*

*/

DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<CVirtualFile*>* files);
bool EraserThreadCallback(CLdString fileName, E_THREAD_OPTION op, DWORD dwValue);
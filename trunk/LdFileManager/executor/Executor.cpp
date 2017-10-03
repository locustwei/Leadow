#include "stdafx.h"
#include "../Config.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "Executor.h"

IEraserThreadCallback* g_callback = nullptr;

DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<CVirtualFile*>* files)
{
	g_callback = callback;

	CLdString param = CMD_ERASE_FILE;
	param.Format(
		_T("%s %s:%d %s:%d"),
		CMD_ERASE_FILE, EPN_MOTHED,
		AppConfig->GetFileErasureMothed(),
		EPN_UNDELFOLDER,
		AppConfig->IsRemoveFolder() ? 1 : 0);

	for (int i = 0; i < files->GetCount();i++)
	{
		param += ' ';
		param += EPN_FILE;
		param += ':';
		param += '\"';
		param += files->Get(i)->GetFullName();
		param += '\"';
	}

	ThisApp->RunInvoker(param, 0, 0);

	return 0;
}

bool EraserThreadCallback(CLdString fileName, E_THREAD_OPTION op, DWORD dwValue)
{
	return g_callback->EraserThreadCallback(nullptr, op, dwValue);
}

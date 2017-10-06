#include "stdafx.h"
#include "../Config.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "Executor.h"


DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<CVirtualFile*>* files)
{
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

	return ThisApp->RunInvoker(param, 0, (PVOID)callback);
}

bool EraserThreadCallback(CLdString fileName, E_THREAD_OPTION op, DWORD dwValue)
{
	return false;// g_callback->EraserThreadCallback(nullptr, op, dwValue);
}

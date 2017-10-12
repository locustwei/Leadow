#include "stdafx.h"
#include "../LdApp/LdStructs.h"
#include "Executor.h"
#include "../../LdApp/LdApp.h"


DWORD ExecuteFileErase(IEraserThreadCallback* callback, CLdArray<TCHAR*>* files)
{
	CLdString param = CMD_ERASE_FILE;
	param.Format(
		_T("%s %s:%d %s:%d"),
		CMD_ERASE_FILE, EPN_MOTHED,
		ThisApp->GetConfig()->GetFileErasureMothed(),
		EPN_UNDELFOLDER,
		ThisApp->GetConfig()->IsRemoveFolder() ? 1 : 0);

	for (int i = 0; i < files->GetCount();i++)
	{
		param += ' ';
		param += EPN_FILE;
		param += ':';
		param += '\"';
		param += files[i];
		param += '\"';
	}

	return ThisApp->RunInvoker(param, 0, (PVOID)callback);
}

bool EraserThreadCallback(CLdString fileName, E_THREAD_OPTION op, DWORD dwValue)
{
	return false;// g_callback->EraserThreadCallback(nullptr, op, dwValue);
}

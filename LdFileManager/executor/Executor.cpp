#include "stdafx.h"
#include "Executor.h"
#include "../Config.h"

DWORD ExecuteFileErase(CLdArray<CVirtualFile*>* files)
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

	CLdApp::ThisApp->RunInvoker(param, 0, 0);

	return 0;
}

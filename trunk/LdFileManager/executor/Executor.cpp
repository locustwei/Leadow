#include "stdafx.h"
#include "../LdApp/LdStructs.h"
#include "../../LdApp/LdApp.h"


DWORD ExecuteFileErase(IEraserListen* callback, CLdArray<TCHAR*>* files)
{
	CLdString param;
	param.Format(
		_T(" %s %s:%d %s:%d"),
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
		param += files->Get(i);
		param += '\"';
	}

	return ThisApp->RunInvoker(param, 0, (PVOID)callback);
}

DWORD ExecuteFileAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files)
{
	CLdString param;
	param.Format(
		_T(" %s %s:%d %s:%d"),
		CMD_ERASE_ANALY, EPN_MOTHED,
		ThisApp->GetConfig()->GetFileErasureMothed(),
		EPN_UNDELFOLDER,
		ThisApp->GetConfig()->IsRemoveFolder() ? 1 : 0);

	for (int i = 0; i < files->GetCount(); i++)
	{
		param += ' ';
		param += EPN_FILE;
		param += ':';
		param += '\"';
		param += files->Get(i);
		param += '\"';
	}

	return ThisApp->RunInvoker(param, 0, (PVOID)callback);
}

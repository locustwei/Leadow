#include "stdafx.h"
#include "../LdApp/LdStructs.h"
#include "../../LdApp/LdApp.h"


DWORD ExecuteFileErase(IEraserListen* callback, CLdArray<TCHAR*>* files)
{
	CLdString param;
	param.Format(
		_T(" ") CMD_ERASE _T(" ") EPN_ERASE_FILES _T(" %s:%d %s:%d"),
		EPN_MOTHED,
		ThisApp->GetConfig()->GetFileErasureMothed(),
		EPN_UNDELFOLDER,
		ThisApp->GetConfig()->IsRemoveFolder() ? 1 : 0);

	for (int i = 0; i < files->GetCount();i++)
	{
		param += ' ';
		param += EPN_NAME;
		param += ':';
		param += '\"';
		param += files->Get(i);
		param += '\"';
	}

	return ThisApp->RunPlugin(param, 0, (PVOID)callback);
}

DWORD ExecuteFileAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* files)
{
	CLdString param = _T(" ") CMD_ERASE _T(" ") CMD_ANALY_FILES;

	for (int i = 0; i < files->GetCount(); i++)
	{
		param += ' ';
		param += EPN_NAME;
		param += ':';
		param += '\"';
		param += files->Get(i);
		param += '\"';
	}

	return ThisApp->RunPlugin(param, 0, (PVOID)callback);
}

DWORD ExecuteVolumeAnalysis(IEraserListen* callback, CLdArray<TCHAR*>* volumes)
{
	CLdString param = _T(" ") CMD_ERASE _T(" ") CMD_ANALY_VOLUMES;

	for (int i = 0; i < volumes->GetCount(); i++)
	{
		param += ' ';
		param += EPN_NAME;
		param += ':';
		param += volumes->Get(i);
	}

	return ThisApp->RunPlugin(param, RUN_FLAG_ASADMINI, (PVOID)callback);
}

#include "stdafx.h"
#include "RunErasure.h"
#include "../Library.h"
#include "EraserLinstenImpl.h"

#pragma warning(disable:4996)

bool AnalEraseFileParam(LPWSTR* lpParams, int nParamCount, CDynObject& Params)
{
	int mothed;
	for (int i = 0; i<nParamCount; i++)
	{
		if (wcsnicmp(lpParams[i], EPN_ERASE_FILES, wcslen(EPN_ERASE_FILES)) == 0)
		{
			Params.AddObjectAttribute(_T("AC"), 1);
		}
		else if (wcsnicmp(lpParams[i], ENP_ERASE_VOLUMES, wcslen(ENP_ERASE_VOLUMES)) == 0)
		{
			Params.AddObjectAttribute(_T("AC"), 2);
		}
		else if (wcsnicmp(lpParams[i], CMD_ANALY_FILES, wcslen(CMD_ANALY_FILES)) == 0)
		{
			Params.AddObjectAttribute(_T("AC"), 3);
		}
		else if (wcsnicmp(lpParams[i], CMD_ANALY_VOLUMES, wcslen(CMD_ANALY_VOLUMES)) == 0)
		{
			Params.AddObjectAttribute(_T("AC"), 4);
		}
		else if (wcsnicmp(lpParams[i], EPN_MOTHED, wcslen(EPN_MOTHED)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			mothed = CLdStringW::Try2Int(p, -1);
			if (mothed == -1)
				return false;
			Params.AddObjectAttribute(EPN_MOTHED, mothed);
		}
		else if (wcsnicmp(lpParams[i], EPN_NAME, wcslen(EPN_NAME)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			CLdString str = p;
			str.Trim();
			if (str.GetLength() < 3)
				return false;
			if (str[0] == '\"')
			{
				str.Delete(0, 1);
			}
			if (str[str.GetLength() - 1] == '\"')
				str.Delete(str.GetLength() - 1, 1);

			Params.AddArrayValue(EPN_NAME, str.GetData());
		}
		else if (wcsnicmp(lpParams[i], EPN_UNDELFOLDER, wcslen(EPN_UNDELFOLDER)) == 0)
		{
			Params.AddObjectAttribute(EPN_UNDELFOLDER, true);
		}
		else
			return false;
	}
	if (Params.GetDataType(EPN_MOTHED) == 6)
		Params.AddObjectAttribute(EPN_MOTHED, ThisApp->GetConfig()->GetFileErasureMothed());
	if (Params.GetDataType(EPN_UNDELFOLDER) == 6)
		Params.AddObjectAttribute(EPN_UNDELFOLDER, ThisApp->GetConfig()->IsRemoveFolder());
	return true;
}

DWORD RunEraseFile(LPWSTR* lpParams, int nParamCount)
{
	CDynObject Param;

	if (!AnalEraseFileParam(lpParams, nParamCount, Param))
	{
		//goto help;
		return 1;
	}

	CEraserListenImpl* impl = new CEraserListenImpl();
	if(!impl->Connect())
	{
		delete impl;
		return 2;
	}

	IErasureLibrary* Eraser = CLdLibray::LoadErasureLibrary();
	if (!Eraser)
		return 2;

	switch(Param.GetInteger(_T("AC")))
	{
	case 1:
		return Eraser->EraseFile(Param, impl);
	case 2:
		return Eraser->EraseVolume(Param, impl);
	case 3:
		return Eraser->AnaFile(Param, impl);
	case 4:
		return Eraser->AnaVolume(Param, impl);
	default:
		return 3;
	}
}

#include "stdafx.h"
#include "RunErasure.h"
#include "../LdFileEraser/ErasureLibrary.h"
#include "../Library.h"
#include "EraserThreadCallbackImpl.h"

#pragma warning(disable:4996)

bool AnalEraseFileParam(LPWSTR* lpParams, int nParamCount, CLdConfig& Params)
{
	int mothed;
	for (int i = 0; i<nParamCount; i++)
	{
		if (wcsnicmp(lpParams[i], EPN_MOTHED, wcslen(EPN_MOTHED)) == 0)
		{
			LPWSTR p = wcschr(lpParams[i], ':');
			if (!p)
				return false;
			p += 1;
			mothed = CLdStringW::Try2Int(p, -1);
			if (mothed == -1)
				return false;
			Params.AddConfigObject(EPN_MOTHED, mothed);
		}
		else if (wcsnicmp(lpParams[i], EPN_FILE, wcslen(EPN_FILE)) == 0)
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

			Params.AddArrayValue(EPN_FILE, str.GetData());
		}
		else if (wcsnicmp(lpParams[i], EPN_UNDELFOLDER, wcslen(EPN_UNDELFOLDER)) == 0)
		{
			Params.AddConfigObject(EPN_UNDELFOLDER, true);
		}
		else
			return false;
	}

	return true;
}

DWORD RunEraseFile(CLdConfig& Param)
{
	IErasureLibrary* Eraser = CLdLibray::LoadErasureLibrary();
	
	return Eraser->EraseFile(Param, new CEraserThreadCallbackImpl());
}

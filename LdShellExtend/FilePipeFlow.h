/*!
 * \file LdContextMenu.h
 * \date 2017/04/28 11:25
 *
 * \author asa
 * 
 *
 * WindowsÓÒ¼ü²Ëµ¥²å¼þ¡£
 *
 * 
 *
 * 
*/

#pragma once

#include "LdStructs.h"

// CLdContextMenu

class CFilePipeFlow :public IPipeDataProvider
{
public:
	CFilePipeFlow();

	~CFilePipeFlow();

	static BOOL StartProtectFlow(int nFileCount, LPTSTR* lpFiles);
	static BOOL StartDeleteFlow(int nFileCount, LPTSTR* lpFiles);
private:
	TCHAR m_PipeName[30];
	struct FLOW_CONTEXT {
		UINT nStep;
	};

	UINT nCount;
	LPTSTR* lpFiles;
	LD_FUNCTION_ID m_FId;

	virtual PIPE_FOLW_ACTION PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext) override;
};


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

class CFileProtectPipeFlow :public IPipeDataProvider
{
public:
	CFileProtectPipeFlow();

	~CFileProtectPipeFlow();

	static BOOL StartPipeFlow(int nFileCount, LPTSTR* lpFiles);
private:
	TCHAR m_PipeName[30];
	struct FLOW_CONTEXT {
		UINT nStep;
	};

	UINT nCount;
	LPTSTR* lpFiles;

	virtual PIPE_FOLW_ACTION PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext) override;
};


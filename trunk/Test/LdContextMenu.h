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

class CLdContextMenu :
	public IPipeDataProvider
{
public:
	CLdContextMenu()
	{
		m_SelectFiles = NULL;
		m_SelectCount = 0;
	}

	~CLdContextMenu()
	{
		for(int i=0; i<m_SelectCount; i++)
			delete [] m_SelectFiles[i];
		if(m_SelectFiles)
			delete [] m_SelectFiles;
		m_SelectFiles = NULL;
		m_SelectCount = 0;
	}

	virtual HRESULT STDMETHODCALLTYPE InvokeCommand(LD_FUNCTION_ID id);
private:
	LPTSTR* m_SelectFiles;
	int m_SelectCount;

	virtual PIPE_FOLW_ACTION PFACallback(PIPE_FOLW_ACTION current, LPVOID& lpBuffer, UINT& nBufferSize, PVOID pContext) override;

	LD_FUNCTION_ID m_FunctionId;
};


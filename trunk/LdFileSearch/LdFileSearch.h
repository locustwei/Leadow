// LdFileSearch.h : LdFileSearch DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLdFileSearchApp
// �йش���ʵ�ֵ���Ϣ������� LdFileSearch.cpp
//

class CLdFileSearchApp : public CWinApp
{
public:
	CLdFileSearchApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


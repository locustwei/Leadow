// LdFileProtect.h : LdFileProtect DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLdFileProtectApp
// �йش���ʵ�ֵ���Ϣ������� LdFileProtect.cpp
//

class CLdFileProtectApp : public CWinApp
{
public:
	CLdFileProtectApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

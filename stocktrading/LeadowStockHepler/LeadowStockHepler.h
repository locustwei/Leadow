// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// LeadowStockHepler.h : LeadowStockHepler Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "ITradInterface.h"
#include "ScriptEng\ScriptEng.h"
#include "..\StockDataAPI\IDataInterface.h"


// CLeadowStockHeplerApp:
// �йش����ʵ�֣������ LeadowStockHepler.cpp
//

class CLeadowStockHeplerApp : public CWinAppEx
{
private:
	void InitScriptEng();

public:
	CLeadowStockHeplerApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	ITradInterface* m_TradClient;
	CScriptEng* m_ScriptEng;
	IDataInterface* m_DateInterface;


	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLeadowStockHeplerApp theApp;

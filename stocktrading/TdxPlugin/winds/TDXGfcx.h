/************************************************************************
�ɷݲ�ѯ����
************************************************************************/
#pragma once
#include "..\..\PublicLib\hooks\WindowHook.h"
#include "..\TDXSocketData.h"

class CTDXZjgf :public CWndHook
{
public:
	CTDXZjgf(HWND hWnd);
	~CTDXZjgf(void);
	int GetZjgf(_Out_ PTDX_STOCK_ZJGF* result);
private:
	HWND m_l628;         // �ʲ�ͳ����Ϣ�����:*  ����:*  �ο���ֵ:*  �ʲ�:*  ӯ��:* 
	HWND m_lst61F;       // ���й�Ʊ�б�
	int GetGf(PTDX_STOCK_GF pGf);
	BOOL GetStatisticsValue(float& ye, float& ky, float& sz, float& yk);
};


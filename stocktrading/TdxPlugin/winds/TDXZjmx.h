/************************************************************************/
/* �ʽ���ϸ���������ݲ�ѯ������
*/
/************************************************************************/
#pragma once
#include "..\..\PublicLib\hooks\WindowHook.h"

class CTDXZjmx :public CWndHook
{
public:
	CTDXZjmx(HWND hWnd);
	~CTDXZjmx(void);
private:
	HWND m_cbBz;
	HWND m_dtBegin;
	HWND m_dtEnd;
	HWND m_btnOk;
	HWND m_lstGp;
};


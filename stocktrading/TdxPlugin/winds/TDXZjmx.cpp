/************************************************************************/
/* �ʽ���ϸ���������ݲ�ѯ������
*/
/************************************************************************/
#include "TDXZjmx.h"
#include "..\..\PublicLib\hooks\WindowHook.h"

CTDXZjmx::CTDXZjmx(HWND hWnd):CWndHook(hWnd)
{
	m_cbBz     = GetDlgItem(hWnd, 0x05D3);    //���֣�ComboBox��
	m_dtBegin  = GetDlgItem(hWnd, 0x0468);	  //��ʼ���ڣ�SysDateTimePick��
	m_dtEnd    = GetDlgItem(hWnd, 0x046B);    //�������ڣ�SysDateTimePick��
	m_btnOk    = GetDlgItem(hWnd, 0x0474);    //��ѯ��Button��
	m_lstGp    = GetDlgItem(hWnd, 0x061F); 	  //���List��SysListView32��
}


CTDXZjmx::~CTDXZjmx(void)
{
}

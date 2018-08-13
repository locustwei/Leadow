#include "TDXStockSell.h"
#include <stdio.h>
#include "..\..\PublicLib\Utils_Wnd.h"


CTDXStockSell::CTDXStockSell(HWND hWnd):CWndHook(hWnd)
{
	m_edCode   = GetDlgItem(hWnd, 0x2EE5);     //��Ʊ���루Edit��
	m_edValue  = GetDlgItem(hWnd, 0x2EE6);	   //�����۸�Edit��
	m_edCount  = GetDlgItem(hWnd, 0x2EE7);     //����������Edit��
	m_btnOk    = GetDlgItem(hWnd, 0x07DA);     //�����µ���Button��
	m_lbJe     = GetDlgItem(hWnd, 0x2EFC);     //���ý�Static��
	m_lbZdsl   = GetDlgItem(hWnd, 0x0811); 	   //������������Static��
	m_lstGp    = GetDlgItem(hWnd, 0x0810); 	   //���й�ƱList��SysListView32��
	m_cbBjfs   = GetDlgItem(hWnd, 0x046D);     //���۷�ʽ��ComboBox��
	m_btnAll   = GetDlgItem(hWnd, 0x05D7); 	   //ȫ����Button��
}


CTDXStockSell::~CTDXStockSell(void)
{
}

LRESULT CTDXStockSell::WndPROC( HWND hwnd, UINT nCode,WPARAM wparam,LPARAM lparam )
{
	switch(nCode){
	case MM_STOCK_SELL_END:
		TradSocket->SendStockSellResult(m_HtId);
		break;
	}
	return CWndHook::WndPROC(hwnd, nCode, wparam, lparam);
}

BOOL CTDXStockSell::DoSell( STOCK_MARK mark, LPCSTR szSymbol, float fPrice, DWORD dwVolume )
{
	BOOL result = FALSE;
	m_HtId = 0;
	m_Mark = mark;
	strcpy_s(m_Code, szSymbol);
	m_Price = fPrice;
	m_Volume = dwVolume;

	RunOnViceThread(&SellContolInViceThread, (LPARAM)this);//�ڸ����߳��еȴ�����ȷ�϶Ի���

	return result;
}

void CTDXStockSell::SetControlsText()
{
	SetWindowTextA(m_edCode, m_Code);
	char szPrice[10] = {0};
	sprintf_s(szPrice, "%.2f", m_Price);
	SetWindowTextA(m_edValue, szPrice);
	sprintf_s(szPrice, "%d", m_Volume);
	SetWindowTextA(m_edCount, szPrice);
}

//�ڸ����߳������У������µ�����
#define COMFIRM_DLGTITLE L"��������ȷ��"
#define PROMPT_DLGTITLE L"��ʾ"
void SellContolInViceThread(LPARAM lparam)
{
	CTDXStockSell* dlgSell = (CTDXStockSell*)lparam;

	//������и�ֵ------------------------------------------------------------------------------------------------------
	int i = 0;
	do 
	{//
		dlgSell->SetControlsText();
		Sleep(300);
		if(GetWindowTextLength(dlgSell->m_edCode)==6 && GetWindowTextLength(dlgSell->m_edValue)>0 && GetWindowTextLength(dlgSell->m_edCount)>0)
			break;
	} while (i++<3);

	if(GetWindowTextLength(dlgSell->m_edCode)!=6 || GetWindowTextLength(dlgSell->m_edValue)==0 || GetWindowTextLength(dlgSell->m_edCount)==0) //û�гɹ�
		goto exit;

	//������밴ť------------------------------------------------------------------------------------------------------
	dlgSell->m_btnClicked = FALSE;
	i = 0;
	do 
	{
		Sleep(300);
		SendClickMessage(dlgSell->m_btnOk, TRUE);
	} while (i++<3);

	//�رս���ȷ�϶Ի���--------------------------------------------------------------------------------------------
	i = 0;
	HWND hwnd = NULL; 
	do{
		Sleep(300);
		hwnd = FindWindowEx(NULL, NULL, CN_Dialog, COMFIRM_DLGTITLE);//����ȷ�ϴ���
		if(hwnd){
			break;
		}
	}while(i++<3);

	if(hwnd==NULL)
		goto exit;

	HWND hBtn = GetDlgItem(hwnd, 0x1B67);
	i = 0;  //�п���һ�ε�����ɹ���3��
	do 
	{
		SendClickMessage(hBtn, TRUE);
		Sleep(100);
		hwnd = FindWindowEx(NULL, NULL, CN_Dialog, COMFIRM_DLGTITLE);
		if(!hwnd)
			break;
	} while (i++<3);

	if(hwnd)  //������ڻ���˵���رղ��ɹ�
		goto exit;

	//�رպ�ͬ����ʾ�Ի���----------------------------------------------------------------------------------------
	i = 0;
	do{
		Sleep(500);
		hwnd = FindWindowEx(NULL, NULL, CN_Dialog, PROMPT_DLGTITLE); //�ύί�к󣬵�������ʾ������
		if(hwnd){
			dlgSell->m_HtId = 1;  //todo ��ȡ��ͬ��
			hBtn = GetDlgItem(hwnd, 0x1B67);
			if(hBtn)
				SendClickMessage(hBtn, TRUE);
			break;
		}
	}while (i++<3);	

exit:
	PostMessage(dlgSell->m_hWnd, MM_STOCK_SELL_END, 0, 0);

}

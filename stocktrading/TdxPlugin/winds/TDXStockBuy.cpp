/************************************************************************/
/* ��Ʊ���봰��
*/
/************************************************************************/
#include "TDXStockBuy.h"
#include <stdio.h>
#include "..\..\PublicLib\Utils_Wnd.h"
#include "..\stdafx.h"

CTDXStockBuy::CTDXStockBuy(HWND hWnd):CWndHook(hWnd)
{
	m_Inited = FALSE;

	m_edCode   = GetDlgItem(hWnd, 0x2EE5);     //��Ʊ���루Edit��
	m_edValue  = GetDlgItem(hWnd, 0x2EE6);	   //����۸�Edit��
	m_edCount  = GetDlgItem(hWnd, 0x2EE7);     //����������Edit��
	m_btnOk    = GetDlgItem(hWnd, 0x07DA);     //�����µ���Button��
	m_lbJe     = GetDlgItem(hWnd, 0x2EFC);     //���ý�Static��
	m_lbZdsl   = GetDlgItem(hWnd, 0x07E6); 	   //������������Static��
	m_lstGp    = GetDlgItem(hWnd, 0x0810); 	   //���й�ƱList��SysListView32��
	m_cbBjfs   = GetDlgItem(hWnd, 0x046D);     //���۷�ʽ��ComboBox��
	m_btnAll   = GetDlgItem(hWnd, 0x05D7); 	   //ȫ����Button��
}


CTDXStockBuy::~CTDXStockBuy(void)
{
}

LRESULT CTDXStockBuy::WndPROC( HWND hwnd, UINT nCode,WPARAM wparam,LPARAM lparam )
{
	LRESULT result = CWndHook::WndPROC(hwnd, nCode, wparam, lparam);
	switch(nCode){
	case MM_STOCK_BY_END:
		TradSocket->SendStockByResult(m_HtId);
		break;
	}
	return result;
}

BOOL CTDXStockBuy::DoBy( STOCK_MARK mark, LPCSTR szSymbol, float fPrice, DWORD dwVolume )
{
	BOOL result = FALSE;
	m_HtId = 0;
	m_Mark = mark;
	strcpy_s(m_Code, szSymbol);
	m_Price = fPrice;
	m_Volume = dwVolume;
	
	RunOnViceThread(&ByContolInViceThread, (LPARAM)this);//�ڸ����߳��еȴ�����ȷ�϶Ի���

	return result;
}

void CTDXStockBuy::SetControlsText()
{
	SetWindowTextA(m_edCode, m_Code);
	char szPrice[10] = {0};
	sprintf_s(szPrice, "%.2f", m_Price);
	SetWindowTextA(m_edValue, szPrice);
	sprintf_s(szPrice, "%d", m_Volume);
	SetWindowTextA(m_edCount, szPrice);
}

//�ڸ����߳������У������µ�����
#define COMFIRM_DLGTITLE L"���뽻��ȷ��"
#define PROMPT_DLGTITLE L"��ʾ"
void ByContolInViceThread(LPARAM lparam)
{
	CTDXStockBuy* dlgBy = (CTDXStockBuy*)lparam;
	
	//������и�ֵ------------------------------------------------------------------------------------------------------
	int i = 0;
	do 
	{//
		dlgBy->SetControlsText();
		Sleep(300);
		if(GetWindowTextLength(dlgBy->m_edCode)==6 && GetWindowTextLength(dlgBy->m_edValue)>0 && GetWindowTextLength(dlgBy->m_edCount)>0)
			break;
	} while (i++<3);

	if(GetWindowTextLength(dlgBy->m_edCode)!=6 || GetWindowTextLength(dlgBy->m_edValue)==0 || GetWindowTextLength(dlgBy->m_edCount)==0) //û�гɹ�
		goto exit;
	
	//������밴ť------------------------------------------------------------------------------------------------------
	dlgBy->m_btnClicked = FALSE;
	i = 0;
	do 
	{
		Sleep(300);
		SendClickMessage(dlgBy->m_btnOk, TRUE);
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
			dlgBy->m_HtId = 1;  //todo ��ȡ��ͬ��
			hBtn = GetDlgItem(hwnd, 0x1B67);
			if(hBtn)
				SendClickMessage(hBtn, TRUE);
			break;
		}
	}while (i++<3);	

exit:
	PostMessage(dlgBy->m_hWnd, MM_STOCK_BY_END, 0, 0);

}

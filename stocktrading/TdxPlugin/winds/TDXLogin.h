/*
ͨ�������Ͻ��׵�¼����
*/

#pragma once
#include "..\stdafx.h"
#include "..\..\PublicLib\hooks\WindowHook.h"

//��¼���ڿؼ�DlgItemID
#define ID_ZHLX = 0x044B      //�˺����ͣ��ʽ��˺ţ�
#define ID_DLFWQ = 0x044F	  //��¼��������        
#define ID_ZH = 0x044C		  //�˺�                
#define ID_QD = 0x0001		  //ȷ����ť            
#define ID_QX = 0x0002		  //ȡ����ť            
#define ID_MM = 0x0450		  //����                
#define ID_AQFS = 0x0457	  //��ȫ��ʽ            
#define ID_YZM = 0x0458		  //��֤��              

class CTDXLogin :public CWndHook
{
private:
	HWND m_CbAType;  //�˺����ͣ��ʽ��˺ţ�  0x044B
	HWND m_CbSever;  //��¼��������          0x044F
	HWND m_CbZH;     //�˺�                  0x044C
	HWND m_BtOk;     //ȷ����ť              0x0001
	HWND m_BtQX;     //ȡ����ť              0x0002
	HWND m_EdMM;     //����                  0x0450
	HWND m_CbJYFS;   //��ȫ��ʽ              0x0457
	HWND m_EdYZM;    //��֤��                0x0458
protected:
	virtual LRESULT WndPROC(HWND hwnd, UINT nCode,WPARAM wparam,LPARAM lparam);
public:
	CTDXLogin(HWND hWnd);
	~CTDXLogin(void);
	BOOL getYZM(LPTSTR szCode);       //��ȡ��֤��
public:
	static CTDXLogin* WndHooker;
	static void HookLoginWnd(HWND hwnd);
};


/*
���������Ч��
1���ö�
2���Զ�����
*/

#pragma once

#include "..\stdAfx.h"
#include "WindowHook.h"

#define WNDEFCT_HM_NONE     0   //������
#define WNDEFCT_HM_TOP      1   //��������
#define WNDEFCT_HM_BOTTOM   2   //��������
#define WNDEFCT_HM_LEFT     3   //��������
#define WNDEFCT_HM_RIGHT    4   //��������

#define WNDEFCT_CM_ELAPSE   200 //�������Ƿ��뿪���ڵ�ʱ����
#define WNDEFCT_HS_ELAPSE   5   //���ػ���ʾ����ÿ����ʱ����
#define WNDEFCT_HS_STEPS    10  //���ػ���ʾ���̷ֳɶ��ٲ�

#define WNDEFCT_INTERVAL    20  //����ճ��ʱ�������Ļ�߽����С���,��λΪ����
#define WNDEFCT_INFALTE     10  //��������ʱ����봰�ڱ߽����С���,��λΪ����
#define WNDEFCT_MINCX       200 //������С���
#define WNDEFCT_MINCY       400 //������С�߶�

class CWndEffect : public CWndHook
{
public:
	CWndEffect(HWND hwnd);	
	BOOL StartHook();
	
protected:
	//�����ƶ�ʱ���ڵĴ�С
	void FixMoving(UINT_PTR fwSide, LPRECT pRect);
	//�����ĸı䴰�ڴ�Сʱ���ڵĴ�С
	void FixSizing(UINT_PTR fwSide, LPRECT pRect);
	//������״̬��ʾ����
	void DoShow();
	void DoHide();
	//���غ���,ֻ��Ϊ�˷������
	BOOL MySetWindowPos(LPCRECT pRect, UINT nFlags= SWP_SHOWWINDOW|SWP_NOSIZE);
	//BOOL OnInitDialog();
	//void OnPaint();
	//HCURSOR OnQueryDragIcon();
	void OnNcHitTest(int x, int y);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSizing(UINT_PTR fwSide, LPRECT pRect);
	//virtual int DoInitWnd();
	void OnMoving(UINT_PTR fwSide, LPRECT pRect);
	virtual LRESULT WndPROC(HWND hwnd, UINT nCode,WPARAM wparam,LPARAM lparam);
	
private:
	HICON m_hIcon;
	//size �����
	//BOOL m_isSizeChanged;   //���ڴ�С�Ƿ�ı��� 
	BOOL m_isSetTimer;      //�Ƿ������˼������Timer
	
	//INT  m_oldWndHeight;    //�ɵĴ��ڿ��
	INT  m_taskBarHeight;   //�������߶�
	INT  m_edgeHeight;      //��Ե�߶�
	INT  m_edgeWidth;       //��Ե���

	INT  m_hideMode;        //����ģʽ
	BOOL m_hsFinished;      //���ػ���ʾ�����Ƿ����
    BOOL m_hiding;          //�ò���ֻ����!m_hsFinished����Ч
	                        //��:��������,��:������ʾ
	BOOL m_topMost;         //�Ƿ�ʹ��TopMost���
	BOOL m_useSteps;        //�Ƿ�ʹ�ó���Ч��
	BOOL m_toolWnd;         //�Ƿ�ʹ��Tool Window ���
	
public:
};

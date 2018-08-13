// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once


#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include "windows.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "resource.h"
#include "TdxTradSocket.h"

#define MM_CUSTOM WM_USER + 0xA0CD
#define MM_RUNONMAINTHREAD MM_CUSTOM + 1      //���������̷߳���Ϣ���Ա��ô������������߳̿ռ䡣wprarm: ����ָ�룻lparam������ ����RunOnMainThread
#define MM_LOGINED MM_CUSTOM + 2              //��¼���������ڷ�����Ϣ  
#define MM_STOCK_BY_END MM_CUSTOM + 3         //��Ʊ�����µ���������봰�ڷ�����Ϣ
#define MM_STOCK_SELL_END MM_CUSTOM + 4       //��Ʊ�����µ�������������ڷ�����Ϣ

extern HINSTANCE hInstance;  //��̬��handle
extern DWORD dwViceThreadId; //���߳�ID
extern HWND hMainWnd;
extern CTdxTradSocket* TradSocket;

typedef void (* RunThreadFunc)(LPARAM lparam); 

//�ڸ����߳���ִ�У��紦��ģ̬�Ի���
inline void RunOnViceThread(RunThreadFunc func, LPARAM lparam)
{
	PostThreadMessage(dwViceThreadId, WM_USER, (WPARAM)func, lparam); 
}
//�����߳�ִ�У������������ض����������߳���ִ�У�
inline void RunOnMainThread(RunThreadFunc func, LPARAM lparam)
{
	PostMessage(hMainWnd, MM_RUNONMAINTHREAD, (WPARAM)func, lparam); 
}
/*!
 * \file LdInvoker.cpp
 * \date 2017/04/26 14:22
 *
 * asa
 * 
 * Invoker ���ڼ��ز����ö�̬�⺯����
 * Explorer �������ֱ��ִ�ж�̬�⺯�����������ԱȨ�ޣ�
 * ��Ϊ�˱�����ռ��̫����Դ����������ִ�й��ܡ�
 * 
 *
 * 
*/

#include "stdafx.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include "LdStructs.h"
#include <string.h>
#include "LdContextMenu.h"
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CLdContextMenu menu;
	menu.InvokeCommand(LFI_HIDE_FILE);

	return (int) 0;
}

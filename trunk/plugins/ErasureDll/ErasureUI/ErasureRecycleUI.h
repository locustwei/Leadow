/*
��������վ���ļ���
����վ��һ������Ŀ¼��ÿ���̷�����һ������վĿ¼
��ͬWindows����ϵͳ�Ļ���վĿ¼��ͬ��
ϵͳÿɾ��һ���ļ����ڻ���վ���������ļ���
һ����¼ԭ�ļ���Ϣ������Explorer��ʾ����һ������������ԭ���ļ���
������SHGetFileInfo�������ļ���Ӧ������


*/

#pragma once

#include "ErasureFileUI.h"

class CErasureRecycleUI : 
	public CErasureFileUI,
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:

	void EnumRecyleFiels();             //ö�ٻ���վ�������ļ�

protected:
	void AttanchControl(CControlUI* pCtrl) override;
	//�����̻߳ص�����
	//FindFirst ����վʵ���ļ�
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	//����վ���ļ���Ϣ
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö�ٴ���
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	bool GetViewHeader() override;

};


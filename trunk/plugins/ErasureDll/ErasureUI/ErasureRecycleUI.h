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
	public CErasureFileUI
	//IGernalCallback<LPWIN32_FIND_DATA>  //����վʵ���ļ�
	//IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
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
	BOOL EnumRecycleFile_Callback(PVOID data, UINT_PTR Param);
	//����վ���ļ���Ϣ
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//ö�ٴ���
	BOOL EnumVolume_Callback(PVOID pData, UINT_PTR Param);
	bool GetViewHeader() override;

};


#pragma once
#include "ErasureThread.h"
#include "ShFileView.h"


class LDLIB_API CErasureRecycleUI : 
	public CShFileViewUI,
	//public CFramWnd, 
	IGernalCallback<PERASE_CALLBACK_PARAM>,
	IGernalCallback<LPWIN32_FIND_DATA>,  //����վʵ���ļ�
	IGernalCallback<TCHAR*>              //ö�ٴ��̣�����·����
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles; //����վ�е�ʵ���ļ�����ʾ��UI�еĲ��������ļ���
	CEreaserThrads m_EreaserThreads;  //��������߳�

	virtual void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	BOOL GernalCallback_Callback(PERASE_CALLBACK_PARAM pData, UINT_PTR Param) override;

	//FindFirst ����վʵ���ļ�
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//FindFirstVolume ö�ٴ��̣�����·����
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
};


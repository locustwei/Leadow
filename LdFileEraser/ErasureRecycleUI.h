#pragma once
#include "ErasureThread.h"
#include "ShFileView.h"


class LDLIB_API CErasureRecycleUI : 
	public CShFileViewUI,
	//public CFramWnd, 
	IGernalCallback<PERASE_CALLBACK_PARAM>,
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles; //回收站中的实际文件（显示在UI中的不是真正文件）
	CEreaserThrads m_EreaserThreads;  //管理擦除线程

	virtual void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	BOOL GernalCallback_Callback(PERASE_CALLBACK_PARAM pData, UINT_PTR Param) override;

	//FindFirst 回收站实际文件
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//FindFirstVolume 枚举磁盘（按卷路径）
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
};


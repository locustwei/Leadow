#pragma once
#include "ErasureThread.h"


class LDLIB_API CErasureRecycleUI : 
	public CFramWnd, 
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<CLdArray<TCHAR*>*>,  //回收站显示文件
	IGernalCallback<PSH_HEAD_INFO>,      //回收站文件显示列信息
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CListUI* lstFile;
	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView 列头
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles; //回收站中的实际文件（显示在UI中的不是真正文件）
	CEreaserThrads m_EreaserThreads;  //管理擦除线程

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	void AddLstViewHeader(int ncount);
	//FindFirst 回收站实际文件
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	//ShGetFileInfo 回收站显示文件
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//枚举ShellView列头信息
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
	//FindFirstVolume 枚举磁盘（按卷路径）
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);
};


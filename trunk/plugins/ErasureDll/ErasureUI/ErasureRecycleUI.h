/*
擦除回收站的文件。
回收站是一个虚拟目录，每个盘符下有一个回收站目录
不同Windows操作系统的回收站目录不同。
系统每删除一个文件会在回收站产生两个文件，
一个记录原文件信息用于在Explorer显示，另一个被重命名的原来文件，
程序用SHGetFileInfo把两个文件对应起来。


*/

#pragma once

#include "ErasureFileUI.h"

class CErasureRecycleUI : 
	public CErasureFileUI,
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:

	void EnumRecyleFiels();             //枚举回收站的所有文件

protected:
	void AttanchControl(CControlUI* pCtrl) override;
	//擦除线程回掉函数
	//FindFirst 回收站实际文件
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	//回收站的文件信息
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//枚举磁盘
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	bool GetViewHeader() override;

};


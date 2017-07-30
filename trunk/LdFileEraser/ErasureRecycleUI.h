/*
擦除回收站的文件。
回收站是一个虚拟目录，每个盘符下有一个回收站目录
不同Windows操作系统的回收站目录不同。
系统每删除一个文件会在回收站产生两个文件，
一个记录原文件信息用于在Explorer显示，另一个被重命名的原来文件，
程序用SHGetFileInfo把两个文件对应起来。

*/

#pragma once

#include "ErasureThread.h"
#include "ShFileView.h"

class LDLIB_API CErasureRecycleUI : 
	public CShFileViewUI,
	//public CFramNotifyPump, 
//	IFindCompare<PERASURE_FILE_DATA>,    //文件搜索比较回掉
//	ISortCompare<PERASURE_FILE_DATA>,    //文件名排序比较函数。
	IEraserThreadCallback,  //文件擦除线程回掉函数，报告擦除状态、进度信息。
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureRecycleUI();
	virtual ~CErasureRecycleUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOk;
	CFolderInfo m_RecycleFile;
//	CTreeNodes<PERASURE_FILE_DATA> m_RecycleFiles;//回收站中的实际文件（显示在UI中的不是真正文件）
	CEreaserThrads m_EreaserThreads;  //管理擦除线程

	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //擦除状态
		DWORD        nErrorCode;          //错误代码（如果错误）
		CControlUI* ui;                   //listView 行
		DWORD nCount;                     //文件夹下的总的文件数
		DWORD nErasued;                   //已经被擦除的文件数
	}*PFILE_ERASURE_DATA;


	void StatErase();
	virtual void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;
	void EnumRecyleFiels();  
	void FreeRecycleFiles(CLdArray<CVirtualFile*>* files);
	DWORD SetFolderFilesData(CLdArray<CVirtualFile*>* files);
	void DeleteErasuredFile(CLdArray<CVirtualFile*>* files);
protected:
	void AttanchControl(CControlUI* pCtrl) override;

	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
	//FindFirst 回收站实际文件
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//FindFirstVolume 枚举磁盘（按卷路径）
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	//数组接口
//	int ArraySortCompare(ERASURE_FILE_DATA** it1, ERASURE_FILE_DATA** it2) override;
//	int ArrayFindCompare(PVOID key, ERASURE_FILE_DATA** it) override;

};


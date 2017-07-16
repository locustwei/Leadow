#pragma once
#include "ErasureThread.h"
#include <minwindef.h>


class LDLIB_API CShFileViewUI : 
	public CFramWnd, 
	IThreadRunable,                      //文件擦除线程执行代码
	IGernalCallback<LPVOID>,             //擦除完成删除ListUI行（Send2MainThread）   
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<CLdArray<TCHAR*>*>,  //回收站显示文件
	IGernalCallback<PSH_HEAD_INFO>       //回收站文件显示列信息
{
public:
	CShFileViewUI();
	virtual ~CShFileViewUI() override;

	void AddRecord(CLdArray<TCHAR*>* values);
	DWORD AddFile(TCHAR* lpFullName);
	DWORD AddFolder(TCHAR* lpFullName);
	DWORD AddFolder(int nFolderCSIDL);

	DUI_DECLARE_MESSAGE_MAP()

private:
	boolean m_HeaderAdded;
	TCHAR* m_ItemSkin;

	CListUI* lstFile;
	CLdArray<PSH_HEAD_INFO> m_Columes;   //ShellView 列头
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles; //回收站中的实际文件（显示在UI中的不是真正文件）
	CEreaserThrads m_EreaserThreads;  //管理擦除线程

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);

protected:
	virtual void OnInit() override;
	void AddLstViewHeader(int ncount);
	//主线程中删除已擦除的文件对应lstFile行
	BOOL GernalCallback_Callback(LPVOID pData, UINT_PTR Param) override;
	//FindFirst 回收站实际文件
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	//ShGetFileInfo 回收站显示文件
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	//枚举ShellView列头信息
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);

	virtual VOID ThreadRun(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;

};


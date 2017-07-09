#pragma once
#include "ErasureThread.h"

class LDLIB_API CErasureFileUI : 
	public CFramWnd, 
	IThreadRunable,                      //文件擦除线程执行代码
	IGernalCallback<LPVOID>,             //擦除完成删除ListUI行（Send2MainThread）   
	IGernalCallback<LPWIN32_FIND_DATA>,  //回收站实际文件
	IGernalCallback<CLdArray<TCHAR*>*>,  //回收站显示文件
	IGernalCallback<PSH_HEAD_INFO>,      //回收站文件显示列信息
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureFileUI();
	virtual ~CErasureFileUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	CButtonUI* btnOpenFile;
	CButtonUI* btnOk;
	CListUI* lstFile;
	CEreaserThrads m_EreaserThreads;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(CLdString& filename);

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);
protected:
	virtual void OnInit() override;
	
	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};


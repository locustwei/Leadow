#pragma once
#include "ErasureThread.h"

class LDLIB_API CErasureFileUI : 
	public CFramWnd, 
	IThreadRunable,                      //文件擦除线程执行代码
	IGernalCallback<LPVOID>              //擦除完成删除ListUI行（Send2MainThread）
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
	CLdHashMap<PERASURE_FILE_DATA> m_ErasureFiles;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void AddErasureFile(CLdString& filename);

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);
protected:
	virtual void OnInit() override;
	BOOL GernalCallback_Callback(LPVOID pData, UINT_PTR Param) override;
	virtual VOID ThreadRun(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadInit(CThread* Sender, WPARAM Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, WPARAM Param) override;

};


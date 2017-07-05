#pragma once

typedef struct RECYCLE_FILE_DATA
{
	TCHAR cFileName[MAX_PATH];
	bool IsDirectory;
	CControlUI* ListRow;
}*PRECYCLE_FILE_DATA;

#define MAX_THREAD_COUNT 6

class CEreaserThrads: public CLdArray<CThread*>
{
public:
	CEreaserThrads()
	{
		InitializeCriticalSection(&cs);
	}

	void StopThreads()
	{
		EnterCriticalSection(&cs);
		for (int i = 0; i < GetCount(); i++)
			Get(i)->SetTerminatee(true);
		LeaveCriticalSection(&cs);
	};

	CThread* AddThread(IThreadRunable* run, UINT_PTR Param);
	
	void RemoveThread(CThread* thread)
	{
		EnterCriticalSection(&cs);
		Remove(thread);
		LeaveCriticalSection(&cs);
	};
private:
	CRITICAL_SECTION cs;
};

class LDLIB_API CErasureRecycleUI : 
	public CFramWnd, 
	IThreadRunable, 
	IGernalCallback<LPVOID>,             //擦除完成删除ListUI行   
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
	CLdArray<PSH_HEAD_INFO> m_Columes;
	CLdHashMap<PRECYCLE_FILE_DATA> m_RecycledFiles;
	CEreaserThrads m_EreaserThreads;
	LONG m_ThreadCount;

	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);
	void EnumRecyleFiels();

protected:
	virtual void OnInit() override;
	BOOL GernalCallback_Callback(LPVOID pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(LPWIN32_FIND_DATA pData, UINT_PTR Param) override;
	void AddLstViewHeader(int ncount);
	BOOL GernalCallback_Callback(CLdArray<TCHAR*>* pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(PSH_HEAD_INFO pData, UINT_PTR Param) override;
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;

	void ErasureAllFiles(CThread* Sender);
	void ErasureSingleFile(CThread* Sender, TCHAR* Key);
	virtual VOID ThreadRun(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadInit(CThread* Sender, UINT_PTR Param) override;
	virtual VOID OnThreadTerminated(CThread* Sender, UINT_PTR Param) override;

};


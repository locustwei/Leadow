#pragma once
#include "ErasureThread.h"
#include "ShFileView.h"


class CErasureVolumeUI : 
	public CShFileViewUI,
	IEraserThreadCallback,  //文件擦除线程回掉函数，报告擦除状态、进度信息。
	IGernalCallback<TCHAR*>              //枚举磁盘（按卷路径）
{
public:
	CErasureVolumeUI();
	virtual ~CErasureVolumeUI() override;

	DUI_DECLARE_MESSAGE_MAP()

private:
	typedef struct FILE_ERASURE_DATA
	{
		E_FILE_STATE nStatus;             //擦除状态
		DWORD        nErrorCode;          //错误代码（如果错误）
		CControlUI* ui;                   //listView 行
		PVOID pAnalyData;
	}*PFILE_ERASURE_DATA;

	CButtonUI* btnOk;
	CLdArray<CVirtualFile*> m_Volumes;
	CEreaserThrads m_EreaserThreads;  //管理擦除线程

	void StatErase();
	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI &msg) override;
	void OnItemClick(TNotifyUI &msg) override;

protected:
	void AttanchControl(CControlUI* pCtrl) override;
	//FindFirstVolume 枚举磁盘（按卷路径）
	BOOL GernalCallback_Callback(TCHAR* pData, UINT_PTR Param) override;
	bool EraserThreadCallback(CVirtualFile* pFile, E_THREAD_OPTION op, DWORD dwValue) override;
};

